﻿#include <cassert>

#include "VirtualMachine.h"
#include <iostream>
#include <unordered_set>

using namespace GVM;


void VirtualMachine::ExecuteSetupPipeline(Compressed::PipelineSnapshot* ps)
{
    PipelineDescription pipelineDescription;
    static IRenderDevice::RENDERTARGETVIEWHANDLE renderTargets[MAX_RENDERTARGET_ATTACHMENTS];
    static IRenderDevice::UATARGETVIEWHANDLE uaTargets[MAX_RENDERTARGET_ATTACHMENTS];
    static IRenderDevice::VERTEXBUFFERVIEWHANDLE vertexBuffers[32];
    static IRenderDevice::RESOURCEVIEWHANDLE textures[128];
    static IRenderDevice::CONSTBUFFERVIEWHANDLE constBuffers[32];
    static IRenderDevice::RESOURCEVIEWHANDLE texturesNulls[128];
    static IRenderDevice::CONSTBUFFERVIEWHANDLE constBuffersNulls[32];

    pipelinesQueueShift += ps->SnapshotByteSize;

    auto Textures = (ResourceView**)(ps->Data + ps->TexturesShift);
    auto ConstBuffers = (ConstBufferView**)(ps->Data + ps->ConstBuffersShift);

    for (int i = 0; i < ps->texturesNum; i++)
    {
        textures[i] = (IRenderDevice::RESOURCEVIEWHANDLE)resourcesManager.GetRealResourceView(Textures[i]);
    }
    for (int i = 0; i < ps->constBuffersNum; i++)
    {
        constBuffers[i] = (IRenderDevice::CONSTBUFFERVIEWHANDLE)resourcesManager.GetRealResourceView(ConstBuffers[i]);
    }
#ifdef DX11
    RenderDevice->SetupTextures(texturesNulls, 128);
    RenderDevice->SetupRenderTargets(
    nullptr, 8,
        {});
        RenderDevice->SetupUATargets(nullptr, 16);
#endif
    pipelineDescription.CS = resourcesManager.GetRealShader(ps->CS);
    pipelineDescription.PS = resourcesManager.GetRealShader(ps->PS);
    pipelineDescription.VS = resourcesManager.GetRealShader(ps->VS);
    pipelineDescription.DS = resourcesManager.GetRealShader(ps->DS);
    pipelineDescription.HS = resourcesManager.GetRealShader(ps->HS);
    pipelineDescription.GS = resourcesManager.GetRealShader(ps->GS);

    pipelineDescription.samplers = (Compressed::SamplerStateDesc*)(ps->Data + ps->SamplersShift);
    pipelineDescription.samplersNum = ps->samplersNum;

    pipelineDescription.cbCount = ps->constBuffersNum;
    pipelineDescription.srvCount = ps->texturesNum;


    if (ps->VS != nullptr && ps->PS != nullptr)
    {
        pipelineDescription.isCS = false;
        pipelineDescription.rtCount = ps->renderTargetsNum;
        auto RenderTargets = (Compressed::RenderTargetDesc*)(ps->Data + ps->RenderTargetsShift);
        for (int i = 0; i < ps->renderTargetsNum; i++)
        {
            pipelineDescription.blendState.BlendStates[i] = RenderTargets[i].BlendState;
            auto rtvView = resourcesManager.GetResourceView(RenderTargets[i].rtv);
            renderTargets[i] = (IRenderDevice::RENDERTARGETVIEWHANDLE)rtvView.view;
            if (rtvView.rtViewDescription.MakeDefault)
            {
                auto& res = resourcesManager.GetResource(rtvView.resource);
                pipelineDescription.RTVFormats[i] = res.resourceDescription.Format;
            }
            else
                pipelineDescription.RTVFormats[i] = rtvView.rtViewDescription.Format;
        }
        if (ps->depthBufferView)
            pipelineDescription.DSVFormat = resourcesManager.GetResourceView(ps->depthBufferView).dbViewDescription.Format;
        else
            pipelineDescription.DSVFormat = EDepthFormat::FORMAT_UNKNOWN;
        for (int i = ps->renderTargetsNum; i < MAX_RENDERTARGET_ATTACHMENTS; i++)
        {
            pipelineDescription.blendState.BlendStates[i] = Compressed::BlendStateDesc();
            renderTargets[i] = {};
        }
        auto VertexBuffers = (VertexBufferView**)(ps->Data + ps->VertexBuffersShift);
        for (int i = 0; i < ps->vertexBuffersNum; i++)
        {
            vertexBuffers[i] = (IRenderDevice::VERTEXBUFFERVIEWHANDLE)resourcesManager.GetRealResourceView(
                VertexBuffers[i]);
        }

        pipelineDescription.blendState.desc = ps->blendDesc;
        pipelineDescription.viewports = (Compressed::ViewportDesc*)(ps->Data + ps->ViewportsShift);
        pipelineDescription.viewportsNum = ps->viewportsNum;
        pipelineDescription.depthStencilState = ps->depthStencilState;
        pipelineDescription.rasterizerState = ps->rasterizerState;
        pipelineDescription.topology = ps->primitiveTopology;
        pipelineDescription.layout = resourcesManager.GetRealInputLayout(ps->vertexDeclaration);

        RenderDevice->SetupPipeline(pipelineDescription);

        RenderDevice->SetupVertexBuffers((const IRenderDevice::VERTEXBUFFERVIEWHANDLE*)vertexBuffers,
            ps->vertexBuffersNum);
        RenderDevice->SetupIndexBuffer(
            (IRenderDevice::INDEXBUFFERVIEWHANDLE)resourcesManager.GetRealResourceView(ps->indexBufferView));

        RenderDevice->SetupRenderTargets(
            (const IRenderDevice::RENDERTARGETVIEWHANDLE*)renderTargets, ps->renderTargetsNum,
            (IRenderDevice::DEPTHSTENCILVIEWHANDLE)resourcesManager.GetRealResourceView(ps->depthBufferView));
    }
    pipelineDescription.uaCount = ps->uaTargetsNum;

    if (ps->CS)
    {
        pipelineDescription.isCS = true;
        RenderDevice->SetupPipeline(pipelineDescription);

        auto UATargets = (UATargetView**)(ps->Data + ps->UATargetsShift);

        for (int i = 0; i < ps->uaTargetsNum; i++)
        {
            uaTargets[i] = (IRenderDevice::UATARGETVIEWHANDLE)resourcesManager.GetRealResourceView(UATargets[i]);
        }
        for (int i = ps->uaTargetsNum; i < MAX_RENDERTARGET_ATTACHMENTS; i++)
        {
            uaTargets[i] = {};
        }

        RenderDevice->SetupUATargets((IRenderDevice::UATARGETVIEWHANDLE*)uaTargets, ps->uaTargetsNum);
    }


    RenderDevice->SetupConstBuffers(constBuffers, ps->constBuffersNum);
    RenderDevice->SetupTextures(textures, ps->texturesNum);
}

void VirtualMachine::GetPipelineResourceTransitions(Compressed::PipelineSnapshot* ps)
{
    LastPsTransitions.clear();
    //std::unordered_set<Resource*> LastPsReadDepMap, LastPsWriteDepMap;

    auto RenderTargets = (Compressed::RenderTargetDesc*)(ps->Data + ps->RenderTargetsShift);
    auto UATargets = (UATargetView**)(ps->Data + ps->UATargetsShift);


    for (int i = 0; i < ps->renderTargetsNum; i++)
    {
        if (RenderTargets[i].rtv != nullptr)
            LastPsTransitions.push_back(
                MakeTransition(resourcesManager.GetResource(RenderTargets[i].rtv),
                    ResourceStateTransition::DefWriteFlag,
                    GpuResource::ResourceState::RESOURCE_STATE_RENDER_TARGET
                ));
        else
        {
            LastPsTransitions.push_back(
                ResourceStateTransition(
                    ResourceStateTransition::DefInvalidFlag,
                    nullptr,
                    GpuResource::ResourceState::RESOURCE_STATE_RENDER_TARGET,
                    GpuResource::ResourceState::RESOURCE_STATE_RENDER_TARGET
                ));
        }
    }

    for (int i = 0; i < ps->uaTargetsNum; i++)
    {
        if (UATargets[i] != nullptr)
            LastPsTransitions.push_back(
                MakeTransition(resourcesManager.GetResource(UATargets[i]),
                    ResourceStateTransition::DefWriteFlag,
                    GpuResource::ResourceState::RESOURCE_STATE_UNORDERED_ACCESS
                ));
    }


    auto VertexBuffersViews = (VertexBufferView**)(ps->Data + ps->VertexBuffersShift);
    auto TexturesViews = (ResourceView**)(ps->Data + ps->TexturesShift);
    auto ConstBuffersViews = (ConstBufferView**)(ps->Data + ps->ConstBuffersShift);

    for (int i = 0; i < ps->vertexBuffersNum; i++)
    {
        if (VertexBuffersViews[i] != nullptr)
            LastPsTransitions.push_back(
                MakeTransition(resourcesManager.GetResource(VertexBuffersViews[i]),
                    ResourceStateTransition::DefReadFlag,
                    GpuResource::ResourceState::RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
                ));
    }
    for (int i = 0; i < ps->texturesNum; i++)
    {
        if (TexturesViews[i] != nullptr)
            LastPsTransitions.push_back(
                MakeTransition(resourcesManager.GetResource(TexturesViews[i]),
                    ResourceStateTransition::DefReadFlag,
                    GpuResource::ResourceState::RESOURCE_STATE_ALL_SHADER_RESOURCE
                ));
    }
    for (int i = 0; i < ps->constBuffersNum; i++)
    {
        if (ConstBuffersViews[i] != nullptr)
            LastPsTransitions.push_back(
                MakeTransition(resourcesManager.GetResource(ConstBuffersViews[i]),
                    ResourceStateTransition::DefReadFlag,
                    GpuResource::ResourceState::RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
                ));
    }

    if (ps->indexBufferView != nullptr)
        LastPsTransitions.push_back(
            MakeTransition(resourcesManager.GetResource(ps->indexBufferView),
                ResourceStateTransition::DefReadFlag,
                GpuResource::ResourceState::RESOURCE_STATE_INDEX_BUFFER
            ));


    if (ps->depthBufferView != nullptr)
        LastPsTransitions.push_back(
            MakeTransition(resourcesManager.GetResource(ps->depthBufferView),
                ResourceStateTransition::DefReadFlag,
                GpuResource::ResourceState::RESOURCE_STATE_DEPTH_WRITE
            ));
}

void VirtualMachine::PushCommand(EMachineCommands command)
{
    commandQueue.push_back(command);

    switch (command)
    {
    case EMachineCommands::CREATE_RESOURCE:
    {
        auto& resource = resourcesManager.GetResource((Resource*)PullPointer());
        renderGraph.AddCommand(
            {
                EMachineCommands::CREATE_RESOURCE,
                (void*)resource.id
            },
            {
                ResourceStateTransition(
                    ResourceStateTransition::DefInvalidFlag,
                    resource.id,
                    GpuResource::ResourceState::RESOURCE_STATE_GENERIC_READ,
                    GpuResource::ResourceState::RESOURCE_STATE_GENERIC_READ)
            }
        );
        resource.currentState = GpuResource::ResourceState::RESOURCE_STATE_GENERIC_READ;
        resource.realState = GpuResource::ResourceState::RESOURCE_STATE_GENERIC_READ;
        break;
    }

    case EMachineCommands::UPDATE_RESOURCE:
    {
        auto& resource = resourcesManager.GetResource((Resource*)PullPointer());
        renderGraph.AddCommand(
            {
                EMachineCommands::UPDATE_RESOURCE,
                (void*)resource.id
            },
            {
                ResourceStateTransition(
                    ResourceStateTransition::DefInvalidFlag,
                    resource.id,
                    resource.currentState,
                    GpuResource::ResourceState::RESOURCE_STATE_GENERIC_READ)
            }
        );
        resource.currentState = GpuResource::ResourceState::RESOURCE_STATE_GENERIC_READ;
        resource.realState = GpuResource::ResourceState::RESOURCE_STATE_GENERIC_READ;
        break;
    }

    case EMachineCommands::CREATE_RESOURCE_VIEW:
    {
        auto& resourceView = resourcesManager.GetResourceView((ResourceView*)PullPointer());
        renderGraph.AddCommand(
            {
                EMachineCommands::CREATE_RESOURCE_VIEW,
                (void*)resourceView.id
            },
            {}
        );
        break;
    }

    case EMachineCommands::SET_RESOURCE_DATA:
    {
        // auto& resource = resourcesManager.GetResource((Resource*)PullPointer());
        auto& description = PullData<SetResourceDataDesc>();
        PullPointer(description.params.dataSize);
        renderGraph.AddCommand(
            {
                EMachineCommands::SET_RESOURCE_DATA,
                (void*)
                ((uint8_t*)&description - dataQueue.data())
            }, {
                ResourceStateTransition(
                    ResourceStateTransition::DefInvalidFlag,
                    description.resource,
                    GpuResource::ResourceState::RESOURCE_STATE_GENERIC_READ,
                    GpuResource::ResourceState::RESOURCE_STATE_GENERIC_READ)
            }
        );
        break;
    }

    case EMachineCommands::UPLOAD_RESOURCE_DATA:
    {
        // auto& resource = resourcesManager.GetResource((Resource*)PullPointer());
        auto& description = PullData<UploadResourceDataDesc>();
        PullPointer(description.params.dataSize);
        renderGraph.AddCommand(
            {
                EMachineCommands::UPLOAD_RESOURCE_DATA,
                (void*)
                ((uint8_t*)&description - dataQueue.data())
            },
            {
                ResourceStateTransition(
                    ResourceStateTransition::DefInvalidFlag,
                    description.resource,
                    GpuResource::ResourceState::RESOURCE_STATE_GENERIC_READ,
                    GpuResource::ResourceState::RESOURCE_STATE_GENERIC_READ)
            }
        );
        break;
    }
    case EMachineCommands::DRAW:
    {
        auto* ps = (Compressed::PipelineSnapshot*)((pipelinesQueue.data()) + drawCallsQueue[drawCallsQueueShift].positionPS);
        GetPipelineResourceTransitions(ps);

        renderGraph.AddCommand(
            {
                EMachineCommands::DRAW,
                (void*)drawCallsQueueShift
            },
            LastPsTransitions
        );
        drawCallsQueueShift++;
        break;
    }
    case EMachineCommands::CLEAR_PIPELINE:
    {
        //RenderDevice->ClearState();
        break;
    }
    case EMachineCommands::CLEAR_RT:
    {
        auto& desc = PullData<ClearRenderTargetDesc>();

        renderGraph.AddCommand(
            {
                EMachineCommands::CLEAR_RT,
                (void*)
                ((uint8_t*)&desc - dataQueue.data())
            },
            {
                MakeTransition(
                    resourcesManager.GetResource(desc.resourceView),
                    ResourceStateTransition::DefWriteFlag,
                    GpuResource::ResourceState::RESOURCE_STATE_RENDER_TARGET)
            }
        );
        break;
    }
    case EMachineCommands::CLEAR_DS:
    {
        auto& desc = PullData<ClearDepthStencilDesc>();
        renderGraph.AddCommand(
            {
                EMachineCommands::CLEAR_DS,
                (void*)
                ((uint8_t*)&desc - dataQueue.data())
            },
            {
                MakeTransition(
                    resourcesManager.GetResource(desc.resourceView),
                    ResourceStateTransition::DefWriteFlag,
                    GpuResource::ResourceState::RESOURCE_STATE_DEPTH_WRITE)
            }
        );
        break;
    }
    case EMachineCommands::BEGIN_EVENT:
    {
        const char* name = (const char*)dataQueue.data() + queueShift;
        //std::cout << name << std::endl;
        PullPointer(std::strlen(name) + 1);

        renderGraph.AddCommand(
            {
                EMachineCommands::BEGIN_EVENT,
                (void*)
                ((uint8_t*)name - dataQueue.data())
            },
            {}
        );
        break;
    }
    case EMachineCommands::END_EVENT:
    {
        renderGraph.AddCommand(
            {
                EMachineCommands::END_EVENT,
                nullptr
            },
            {}
        );

        break;
    }
    }
}

void VirtualMachine::RunVM()
{
    int comandIndex = 0;

    for (auto& block : renderGraph.Blocks)
    {
        RenderDevice->SyncResourcesState(*block.transitionsBegin);
        RenderDevice->SyncResourcesState(*block.transitionsEnd);

        for (auto& transition : *block.transitionsEnd)
        {
            if (transition.flags != ResourceStateTransition::DefInvalidFlag)
                resourcesManager.GetResource(transition.resource).realState = transition.StateTo;
        }

        for (auto& [command, description] : block.GraphicsNodes)
        {
            comandIndex++;
            switch (command)
            {
            case EMachineCommands::CREATE_RESOURCE:
            {
                auto& resource = resourcesManager.GetResource((Resource*)(description));
                resource.resource = RenderDevice->CreateResource(resource); //todo
                break;
            }

            case EMachineCommands::UPDATE_RESOURCE:
            {
                auto& resource = resourcesManager.GetResource((Resource*)description);
                RenderDevice->DestroyResource(resource.resource);
                resource.resource = RenderDevice->CreateResource(resource);
                break;
            }

            case EMachineCommands::CREATE_RESOURCE_VIEW:
            {
                auto& resourceView = resourcesManager.GetResourceView((ResourceView*)description);
                resourceView.view = RenderDevice->CreateResourceView(resourceView,
                    resourcesManager.GetResource(
                        resourceView.resource));
                break;
            }

            case EMachineCommands::SET_RESOURCE_DATA:
            {
                // auto& resource = resourcesManager.GetResource((Resource*)PullPointer());
                auto& desc = *(SetResourceDataDesc*)(dataQueue.data() + (uint32_t)description);
                RenderDevice->SetSubresourceData(resourcesManager.GetResource(desc.resource),
                    desc.params.dstSubresource,
                    desc.params.rect,
                    dataQueue.data() + desc.shift,
                    desc.params.srcRowPitch,
                    desc.params.srcDepthPitch);
                break;
            }

            case EMachineCommands::UPLOAD_RESOURCE_DATA:
            {
                // auto& resource = resourcesManager.GetResource((Resource*)PullPointer());
                auto& desc = *(UploadResourceDataDesc*)(dataQueue.data() + (uint32_t)description);
                RenderDevice->UploadSubresourceData(resourcesManager.GetResource(desc.resource),
                    desc.params.dstSubresource,
                    desc.params.dataSize,
                    dataQueue.data() + desc.shift,
                    desc.params.srcRowPitch,
                    desc.params.srcDepthPitch);
                break;
            }

            //case EMachineCommands::SETUP_PIPELINE:
            //{
            //   
            //    break;
            //}

            case EMachineCommands::DRAW:
            {
                auto& dcDesc = drawCallsQueue[(uint32_t)description];
                auto* ps = (Compressed::PipelineSnapshot*)((pipelinesQueue.data()) + dcDesc.positionPS);
                ExecuteSetupPipeline(ps);
                //auto drawData = *(DrawCall*)description;
                RenderDevice->Draw(dcDesc.dc);
                drawCallsQueueShift++;
                break;
            }
            case EMachineCommands::CLEAR_PIPELINE:
            {
                RenderDevice->ClearState();
                break;
            }
            case EMachineCommands::CLEAR_RT:
            {
                auto& desc = *(ClearRenderTargetDesc*)(dataQueue.data() + (uint32_t)description);
                RenderDevice->ClearRenderTarget(
                    (IRenderDevice::RENDERTARGETVIEWHANDLE)resourcesManager.GetRealResourceView(desc.resourceView),
                    desc.color);

                break;
            }
            case EMachineCommands::CLEAR_DS:
            {
                auto& desc = *(ClearDepthStencilDesc*)(dataQueue.data() + (uint32_t)description);
                auto* resource = &resourcesManager.GetResource(desc.resourceView);
                RenderDevice->ClearDepthStencil(
                    (IRenderDevice::DEPTHSTENCILVIEWHANDLE)resourcesManager.GetRealResourceView(desc.resourceView),
                    desc.depth,
                    desc.stencil
                );
                break;
            }
            case EMachineCommands::BEGIN_EVENT:
            {
                const char* name = (char*)(dataQueue.data() + (uint32_t)description);
                // std::cout << name << std::endl;
                // PullPointer(std::strlen(name) + 1);
                RenderDevice->BeginEvent(name);
                break;
            }
            case EMachineCommands::END_EVENT:
            {
                RenderDevice->EndEvent();
                break;
            }
            }
        }
        std::cout << std::flush;
    }


    for (auto& resource : resourcesManager.Resources)
    {
        assert(resource.currentState == resource.realState);
    }


    static IRenderDevice::RENDERTARGETVIEWHANDLE renderTargets[1] = {nullptr};
    RenderDevice->SetupRenderTargets((const IRenderDevice::RENDERTARGETVIEWHANDLE*)renderTargets, 1, {});
}


void VirtualMachine::Present()
{
    RenderDevice->Present();
    renderGraph.Clear();
    queueShift = 0;
    pipelinesQueueShift = 0;
    drawCallsQueueShift = 0;
    pipelinesQueue.clear();
    commandQueue.clear();
    dataQueue.clear();
    drawCallsQueue.clear();
}


void VirtualMachine::ResizeBackbuffer(int32_t width, int32_t height)
{
    RenderDevice->ResizeBackbuffer(width, height);
}
