#include "VirtualMachine.h"

using namespace GVM;


void VirtualMachine::ExecuteSetupPipeline()
{
    static Compressed::CoreBlendDesc cBS;
    static IRenderDevice::IRenderTargetView* renderTargets[MAX_RENDERTARGET_ATTACHMENTS];
    static IRenderDevice::IUATargetView* uaTargets[MAX_RENDERTARGET_ATTACHMENTS];
    static IRenderDevice::IVertexBufferView* vertexBuffers[32];
    static IRenderDevice::IResourceView* textures[128];
    static IRenderDevice::IConstBufferView* constBuffers[32];
    Compressed::PipelineSnapshot* ps = (Compressed::PipelineSnapshot*)((pipelinesQueue.data()) + pipelinesQueueShift);
    pipelinesQueueShift += ps->SnapshotByteSize;

    cBS.desc = ps->blendDesc;


    auto RenderTargets = (Compressed::RenderTargetDesc*)(ps->Data+ps->RenderTargetsShift);
    auto UATargets = (UATargetView**)(ps->Data+ps->UATargetsShift);
    
    
    for(int i = 0; i < ps->renderTargetsNum; i++)
    {
        cBS.BlendStates[i] = RenderTargets[i].BlendState;
        renderTargets[i] = (IRenderDevice::IRenderTargetView*)resourcesManager.GetRealResourceView(RenderTargets[i].rtv);
    }
    for(int i = ps->renderTargetsNum; i < MAX_RENDERTARGET_ATTACHMENTS; i++)
    {
        cBS.BlendStates[i] = Compressed::BlendStateDesc();
        renderTargets[i] = nullptr;
    }   
    
    for(int i = 0; i < ps->uaTargetsNum; i++)
    {
        uaTargets[i] = (IRenderDevice::IUATargetView*)resourcesManager.GetRealResourceView(UATargets[i]);
    }
    for(int i = ps->uaTargetsNum; i < MAX_RENDERTARGET_ATTACHMENTS; i++)
    {
        uaTargets[i] = nullptr;
    }


    auto VertexBuffers = (VertexBufferView**)(ps->Data+ps->VertexBuffersShift);
    auto Textures = (ResourceView**)(ps->Data+ps->TexturesShift);
    auto ConstBuffers = (ConstBufferView**)(ps->Data+ps->ConstBuffersShift);
    
    for(int i = 0; i < ps->vertexBuffersNum; i++)
    {
       vertexBuffers[i] = (IRenderDevice::IVertexBufferView*)resourcesManager.GetRealResourceView(VertexBuffers[i]);
    }
    for(int i = 0; i < ps->texturesNum; i++)
    {
       textures[i] = (IRenderDevice::IResourceView*)resourcesManager.GetRealResourceView(Textures[i]);
    }
    for(int i = 0; i < ps->constBuffersNum; i++)
    {
       constBuffers[i] = (IRenderDevice::IConstBufferView*)resourcesManager.GetRealResourceView(ConstBuffers[i]);
    }

    auto Viewports = (Compressed::ViewportDesc*)(ps->Data+ps->ViewportsShift);
    auto Samplers = (Compressed::SamplerStateDesc*)(ps->Data+ps->SamplersShift);

    RenderDevice->ClearState();
    
    RenderDevice->SetupShader(resourcesManager.GetRealShader(ps->CS), EShaderType::COMPUTE_SHADER);
    RenderDevice->SetupShader(resourcesManager.GetRealShader(ps->PS), EShaderType::PIXEL_SHADER);
    RenderDevice->SetupShader(resourcesManager.GetRealShader(ps->VS), EShaderType::VERTEX_SHADER);
    RenderDevice->SetupShader(resourcesManager.GetRealShader(ps->DS), EShaderType::DOMAIN_SHADER);
    RenderDevice->SetupShader(resourcesManager.GetRealShader(ps->HS), EShaderType::HULL_SHADER);
    RenderDevice->SetupShader(resourcesManager.GetRealShader(ps->GS), EShaderType::GEOMETRY_SHADER);

        
    if (ps->VS != nullptr && ps->PS != nullptr)
    {
        RenderDevice->SetupViewports(Viewports,ps->viewportsNum);
        RenderDevice->SetupBlendState(cBS);
        RenderDevice->SetupDepthStencilState(ps->depthStencilState);
        RenderDevice->SetupRasterizerState(ps->rasterizerState);
        RenderDevice->SetupPrimitiveTopology(ps->primitiveTopology);
        RenderDevice->SetupVertexBuffer((const IRenderDevice::IVertexBufferView**)vertexBuffers, ps->vertexBuffersNum);
        RenderDevice->SetupIndexBuffer((IRenderDevice::IIndexBufferView*)resourcesManager.GetRealResourceView(ps->indexBuffer));
        RenderDevice->SetupInputLayout(resourcesManager.GetRealInputLayout(ps->vertexDeclaration));
        
        RenderDevice->SetupRenderTargets(
        (const IRenderDevice::IRenderTargetView**)renderTargets,ps->renderTargetsNum,
        (IRenderDevice::IDepthStencilView*)resourcesManager.GetRealResourceView(ps->DepthBuffer));
    }
    
    if (ps->CS)
    {
        RenderDevice->SetupUATargets((const IRenderDevice::IUATargetView**)uaTargets, ps->renderTargetsNum);
    }
    
    RenderDevice->SetupSamplers(Samplers, ps->samplersNum);
    RenderDevice->SetupConstBuffers(constBuffers,ps->constBuffersNum);
    RenderDevice->SetupTextures(textures, ps->texturesNum);
    
}

void VirtualMachine::Present()
{
    
    for (auto& command : commandQueue)
    {
        switch (command)
        {
        case EMachineCommands::CREATE_RESOURCE:
        {
            auto& resource = resourcesManager.GetResource((Resource*)PullPointer());
            resource.resource = RenderDevice->CreateResource(resource);
            break;
        }

        case EMachineCommands::UPDATE_RESOURCE:
        {
            auto& resource = resourcesManager.GetResource((Resource*)PullPointer());
            RenderDevice->DestroyResource(resource.resource);
            resource.resource = RenderDevice->CreateResource(resource);
            break;
        }
            
        case EMachineCommands::CREATE_RESOURCE_VIEW:
        {
            auto& resourceView = resourcesManager.GetResourceView((ResourceView*)PullPointer());
            resourceView.view = RenderDevice->CreateResourceView(resourceView, resourcesManager.GetResource(resourceView.resource));
            break;
        }
            
        case EMachineCommands::SET_RESOURCE_DATA:
        {
           // auto& resource = resourcesManager.GetResource((Resource*)PullPointer());
            auto& description = PullData<SetResourceDataDesc>();
            const void* data = PullPointer(description.params.dataSize);
            RenderDevice->SetResourceData( resourcesManager.GetResource(description.resource),
                description.params.dstSubresource,
                description.params.rect,
                dataQueue.data()+description.shift,
                description.params.srcRowPitch,
                description.params.srcDepthPitch);
            break;
        }
            
        case EMachineCommands::SETUP_PIPELINE:
        {
            ExecuteSetupPipeline();
            break;
        }
            
        case EMachineCommands::DRAW:
        {
            RenderDevice->Draw(drawCallsQueue[drawCallsQueueShift]);
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
            RenderDevice->ClearRenderTarget((IRenderDevice::IRenderTargetView*)resourcesManager.GetRealResourceView(desc.resource), desc.color);
            break;
        }
        case EMachineCommands::CLEAR_DS:
        {
            auto& desc = PullData<ClearDepthStencilDesc>();
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
            
        }
    }
    RenderDevice->Present();
    
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