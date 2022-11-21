#include "VirtualMachine.h"
#include <iostream>
#include <unordered_set>

using namespace GVM;


void VirtualMachine::ExecuteSetupPipeline(Compressed::PipelineSnapshot* ps)
{
    static Compressed::CoreBlendDesc cBS;
    static IRenderDevice::IRenderTargetView* renderTargets[MAX_RENDERTARGET_ATTACHMENTS];
    static IRenderDevice::IUATargetView* uaTargets[MAX_RENDERTARGET_ATTACHMENTS];
    static IRenderDevice::IVertexBufferView* vertexBuffers[32];
    static IRenderDevice::IResourceView* textures[128];
    static IRenderDevice::IConstBufferView* constBuffers[32];


    pipelinesQueueShift += ps->SnapshotByteSize;

    cBS.desc = ps->blendDesc;


    auto RenderTargets = (Compressed::RenderTargetDesc*)(ps->Data + ps->RenderTargetsShift);
    auto UATargets = (UATargetView**)(ps->Data + ps->UATargetsShift);


    for (int i = 0; i < ps->renderTargetsNum; i++)
    {
        cBS.BlendStates[i] = RenderTargets[i].BlendState;
        renderTargets[i] = (IRenderDevice::IRenderTargetView*)resourcesManager.
            GetRealResourceView(RenderTargets[i].rtv);
    }
    for (int i = ps->renderTargetsNum; i < MAX_RENDERTARGET_ATTACHMENTS; i++)
    {
        cBS.BlendStates[i] = Compressed::BlendStateDesc();
        renderTargets[i] = nullptr;
    }

    for (int i = 0; i < ps->uaTargetsNum; i++)
    {
        uaTargets[i] = (IRenderDevice::IUATargetView*)resourcesManager.GetRealResourceView(UATargets[i]);
    }
    for (int i = ps->uaTargetsNum; i < MAX_RENDERTARGET_ATTACHMENTS; i++)
    {
        uaTargets[i] = nullptr;
    }


    auto VertexBuffers = (VertexBufferView**)(ps->Data + ps->VertexBuffersShift);
    auto Textures = (ResourceView**)(ps->Data + ps->TexturesShift);
    auto ConstBuffers = (ConstBufferView**)(ps->Data + ps->ConstBuffersShift);

    for (int i = 0; i < ps->vertexBuffersNum; i++)
    {
        vertexBuffers[i] = (IRenderDevice::IVertexBufferView*)resourcesManager.GetRealResourceView(VertexBuffers[i]);
    }
    for (int i = 0; i < ps->texturesNum; i++)
    {
        textures[i] = (IRenderDevice::IResourceView*)resourcesManager.GetRealResourceView(Textures[i]);
    }
    for (int i = 0; i < ps->constBuffersNum; i++)
    {
        constBuffers[i] = (IRenderDevice::IConstBufferView*)resourcesManager.GetRealResourceView(ConstBuffers[i]);
    }

    auto Viewports = (Compressed::ViewportDesc*)(ps->Data + ps->ViewportsShift);
    auto Samplers = (Compressed::SamplerStateDesc*)(ps->Data + ps->SamplersShift);

    RenderDevice->ClearState();

    RenderDevice->SetupShader(resourcesManager.GetRealShader(ps->CS), EShaderType::COMPUTE_SHADER);
    RenderDevice->SetupShader(resourcesManager.GetRealShader(ps->PS), EShaderType::PIXEL_SHADER);
    RenderDevice->SetupShader(resourcesManager.GetRealShader(ps->VS), EShaderType::VERTEX_SHADER);
    RenderDevice->SetupShader(resourcesManager.GetRealShader(ps->DS), EShaderType::DOMAIN_SHADER);
    RenderDevice->SetupShader(resourcesManager.GetRealShader(ps->HS), EShaderType::HULL_SHADER);
    RenderDevice->SetupShader(resourcesManager.GetRealShader(ps->GS), EShaderType::GEOMETRY_SHADER);


    if (ps->VS != nullptr && ps->PS != nullptr)
    {
        RenderDevice->SetupViewports(Viewports, ps->viewportsNum);
        RenderDevice->SetupBlendState(cBS);
        RenderDevice->SetupDepthStencilState(ps->depthStencilState);
        RenderDevice->SetupRasterizerState(ps->rasterizerState);
        RenderDevice->SetupPrimitiveTopology(ps->primitiveTopology);
        RenderDevice->SetupVertexBuffer((const IRenderDevice::IVertexBufferView**)vertexBuffers, ps->vertexBuffersNum);
        RenderDevice->SetupIndexBuffer(
            (IRenderDevice::IIndexBufferView*)resourcesManager.GetRealResourceView(ps->indexBuffer));
        RenderDevice->SetupInputLayout(resourcesManager.GetRealInputLayout(ps->vertexDeclaration));

        RenderDevice->SetupRenderTargets(
            (const IRenderDevice::IRenderTargetView**)renderTargets, ps->renderTargetsNum,
            (IRenderDevice::IDepthStencilView*)resourcesManager.GetRealResourceView(ps->DepthBuffer));
    }

    if (ps->CS)
    {
        RenderDevice->SetupUATargets((const IRenderDevice::IUATargetView**)uaTargets, ps->renderTargetsNum);
    }

    RenderDevice->SetupSamplers(Samplers, ps->samplersNum);
    RenderDevice->SetupConstBuffers(constBuffers, ps->constBuffersNum);
    RenderDevice->SetupTextures(textures, ps->texturesNum);
}

void VirtualMachine::GetPipelineResourceDependencies(Compressed::PipelineSnapshot* ps, std::vector<Resource*>& ReadDependencies, std::vector<Resource*>& WrightDependencies)
{
    std::unordered_set<Resource*> readDependencies, wrightDependencies;

    auto RenderTargets = (Compressed::RenderTargetDesc*)(ps->Data + ps->RenderTargetsShift);
    auto UATargets = (UATargetView**)(ps->Data + ps->UATargetsShift);


    for (int i = 0; i < ps->renderTargetsNum; i++)
    {
        wrightDependencies.insert(resourcesManager.
                                  GetResourceView(RenderTargets[i].rtv).resource);
    }

    for (int i = 0; i < ps->uaTargetsNum; i++)
    {
        wrightDependencies.insert(
            resourcesManager.GetResourceView(UATargets[i]).resource);
    }


    auto VertexBuffers = (VertexBufferView**)(ps->Data + ps->VertexBuffersShift);
    auto Textures = (ResourceView**)(ps->Data + ps->TexturesShift);
    auto ConstBuffers = (ConstBufferView**)(ps->Data + ps->ConstBuffersShift);

    for (int i = 0; i < ps->vertexBuffersNum; i++)
    {
        readDependencies.insert(
            resourcesManager.GetResourceView(VertexBuffers[i]).resource);
    }
    for (int i = 0; i < ps->texturesNum; i++)
    {
        readDependencies.insert(
            resourcesManager.GetResourceView(Textures[i]).resource);
    }
    for (int i = 0; i < ps->constBuffersNum; i++)
    {
        readDependencies.insert(
            resourcesManager.GetResourceView(ConstBuffers[i]).resource);
    }

    readDependencies.insert(
        resourcesManager.GetResourceView(ps->indexBuffer).resource);

    for (auto& readDep : readDependencies)
    {
        ReadDependencies.push_back(readDep);
    }
    for (auto& wrightDep : wrightDependencies)
    {
        WrightDependencies.push_back(wrightDep);
    }
}

VirtualMachine::~VirtualMachine()
{
    delete RenderDevice;
}

void VirtualMachine::RunVM()
{
    int comandIndex = 0;
    renderGraph.Clear();

    for (auto& command : commandQueue)
    {
        comandIndex++;

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
                {},
                {resource.id}
            );
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
                {},
                {resource.id}
            );
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
                {},
                {resourceView.resource}
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
                    (void*)&description
                },
                {},
                {description.resource}
            );
            break;
        }

        case EMachineCommands::SETUP_PIPELINE:
        {
            auto* ps = (Compressed::PipelineSnapshot*)((pipelinesQueue.data()) + pipelinesQueueShift);

            static std::vector<Resource*> ReadDep;
            static std::vector<Resource*> WriteDep;
            ReadDep.clear();
            WriteDep.clear();


            GetPipelineResourceDependencies(ps, ReadDep, WriteDep);

            renderGraph.AddCommand(
                {
                    EMachineCommands::SETUP_PIPELINE,
                    (void*)ps
                },
                ReadDep,
                WriteDep
            );

            pipelinesQueueShift += ps->SnapshotByteSize;
            break;
        }

        case EMachineCommands::DRAW:
        {
            renderGraph.AddCommand(
                {
                    EMachineCommands::DRAW,
                    (void*)&drawCallsQueue[drawCallsQueueShift]
                },
                {},
                {}
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
                    (void*)&desc
                },
                {},
                {resourcesManager.GetResourceView(desc.resource).resource}
            );
            break;
        }
        case EMachineCommands::CLEAR_DS:
        {
            auto& desc = PullData<ClearDepthStencilDesc>();
            renderGraph.AddCommand(
                {
                    EMachineCommands::CLEAR_DS,
                    (void*)&desc
                },
                {},
                {resourcesManager.GetResourceView(desc.resource).resource}
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
                    (void*)name
                },
                {},
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
                {},
                {}
            );

            break;
        }
        }
    }



    for (auto& block : renderGraph.Blocks)
    {
        static std::vector<IRenderDevice::IResource*> ReadDep;
        static std::vector<IRenderDevice::IResource*> WriteDep;
        ReadDep.clear();
        WriteDep.clear();
        for (auto* r :  block.ReadDependencies)
        {
            ReadDep.push_back(resourcesManager.GetRealResource(r));
        }
        for (auto* w :  block.WrightDependencies)
        {
            WriteDep.push_back(resourcesManager.GetRealResource(w));
        }
        RenderDevice->SyncResourcesRead(ReadDep.data(), ReadDep.size());
        RenderDevice->SyncResourcesWrite(WriteDep.data(), WriteDep.size());
        
        for (auto& [command, description] : block.Nodes)
        {
            comandIndex++;
            switch (command)
            {
            case EMachineCommands::CREATE_RESOURCE:
            {
                auto& resource = resourcesManager.GetResource((Resource*)description);
                resource.resource = RenderDevice->CreateResource(resource);//todo
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
                    resourcesManager.GetResource(resourceView.resource));
                break;
            }

            case EMachineCommands::SET_RESOURCE_DATA:
            {
                // auto& resource = resourcesManager.GetResource((Resource*)PullPointer());
                auto& desc = *(SetResourceDataDesc*)description;
                RenderDevice->SetResourceData(resourcesManager.GetResource(desc.resource),
                    desc.params.dstSubresource,
                    desc.params.rect,
                    dataQueue.data() + desc.shift,
                    desc.params.srcRowPitch,
                    desc.params.srcDepthPitch);
                break;
            }

            case EMachineCommands::SETUP_PIPELINE:
            {
                auto* ps = (Compressed::PipelineSnapshot*)description;
                ExecuteSetupPipeline(ps);
                break;
            }

            case EMachineCommands::DRAW:
            {
                //auto drawData = *(DrawCall*)description;
                RenderDevice->Draw(*(DrawCall*)description);
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
                auto& desc =*(ClearRenderTargetDesc*)description;
                RenderDevice->ClearRenderTarget(
                    (IRenderDevice::IRenderTargetView*)resourcesManager.GetRealResourceView(desc.resource), desc.color);
                break;
            }
            case EMachineCommands::CLEAR_DS:
            {
                auto& desc = *(ClearDepthStencilDesc*)description;
                //auto& resource = PullData<DepthStencilView*>();
                //auto& depth = PullData<float>();
                //auto& stencil = PullData<uint8_t>();
                RenderDevice->ClearDepthStencil(
                    (IRenderDevice::IDepthStencilView*)resourcesManager.GetRealResourceView(desc.resource),
                    desc.depth,
                    desc.stencil
                );
                break;
            }
            case EMachineCommands::BEGIN_EVENT:
            {
                const char* name = (char*)description;
                std::cout << name << std::endl;
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
    }

    static IRenderDevice::IRenderTargetView* renderTargets[1] = {nullptr};
    RenderDevice->SetupRenderTargets((const IRenderDevice::IRenderTargetView**)renderTargets, 1, nullptr);

    queueShift = 0;
    pipelinesQueueShift = 0;
    drawCallsQueueShift = 0;
    pipelinesQueue.clear();
    commandQueue.clear();
    dataQueue.clear();
    drawCallsQueue.clear();
}

void VirtualMachine::Present()
{
    RenderDevice->Present();
}


void VirtualMachine::ResizeBackbuffer(int32_t width, int32_t height)
{
    RenderDevice->ResizeBackbuffer(width, height);
}
