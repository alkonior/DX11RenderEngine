#include "VirtualMachine.h"

#include "GraphicsVirtualMachine/RenderDeviceDX11/RenderDeviceDX11.h"


using namespace GVM;


void VirtualMachine::ExecuteSetupPipeline()
{
    static Compressed::CoreBlendDesc cBS;
    static IRenderDevice::IRenderTargetView* renderTargets[MAX_RENDERTARGET_ATTACHMENTS];
    static IRenderDevice::IVertexBufferView* vertexBuffers[32];
    static IRenderDevice::IResourceView* textures[128];
    static IRenderDevice::IConstBufferView* constBuffers[32];
    Compressed::PipelineSnapshot* ps = (Compressed::PipelineSnapshot*)((pipelinesQueue.data()) + pipelinesQueueShift);
    pipelinesQueueShift += ps->SnapshotByteSize;

    cBS.desc = ps->blendDesc;
    
    for(int i = 0; i < ps->renderTargetsNum; i++)
    {
        cBS.BlendStates[i] = ps->RenderTargets[i].BlendState;
        renderTargets[i] = (IRenderDevice::IRenderTargetView*)resourcesManager.GetRealResourceView(ps->RenderTargets[i].rtv);
    }
    for(int i = ps->renderTargetsNum; i < MAX_RENDERTARGET_ATTACHMENTS; i++)
    {
        cBS.BlendStates[i] = Compressed::BlendStateDesc();
        renderTargets[i] = (IRenderDevice::IRenderTargetView*)resourcesManager.GetRealResourceView(ps->RenderTargets[i].rtv);
    }
    
    for(int i = 0; i < ps->vertexBuffersNum; i++)
    {
       vertexBuffers[i] = (IRenderDevice::IVertexBufferView*)resourcesManager.GetRealResourceView(ps->VertexBuffers[i]);
    }
    for(int i = 0; i < ps->texturesNum; i++)
    {
       textures[i] = (IRenderDevice::IResourceView*)resourcesManager.GetRealResourceView(ps->Textures[i]);
    }
    for(int i = 0; i < ps->constBuffersNum; i++)
    {
       constBuffers[i] = (IRenderDevice::IConstBufferView*)resourcesManager.GetRealResourceView(ps->ConstBuffers[i]);
    }

    if ( !(-32768.000000 <= ps->Viewports[0].TopLeftX&&
        -32768.000000 <= ps->Viewports[0].TopLeftY&&
        (ps->Viewports[0].TopLeftX+ps->Viewports[0].Width) <= 32767.000000&&
         (ps->Viewports[0].TopLeftY+ps->Viewports[0].Height) <= 32767.000000&&
    0.000000 <= ps->Viewports[0].MinDepth&&
        ps->Viewports[0].MaxDepth <= 1.000000&&
        ps->Viewports[0].MinDepth <= ps->Viewports[0].MaxDepth))
    {
        return;;
    }
    RenderDevice->SetupViewports(ps->Viewports,ps->viewportsNum);
    RenderDevice->SetupBlendState(cBS);
    RenderDevice->SetupDepthStencilState(ps->depthStencilState);
    RenderDevice->SetupRasterizerState(ps->rasterizerState);
    RenderDevice->SetupSamplers(ps->Samplers, ps->samplersNum);
    RenderDevice->SetupPrimitiveTopology(ps->primitiveTopology);
    RenderDevice->SetupVertexBuffer((const IRenderDevice::IVertexBufferView**)vertexBuffers, ps->vertexBuffersNum);
    RenderDevice->SetupIndexBuffer((IRenderDevice::IIndexBufferView*)resourcesManager.GetRealResourceView(ps->indexBuffer));
    RenderDevice->SetupTextures(textures, ps->texturesNum);
    RenderDevice->SetupRenderTargets(
        (const IRenderDevice::IRenderTargetView**)renderTargets,ps->renderTargetsNum,
        (IRenderDevice::IDepthStencilView*)resourcesManager.GetRealResourceView(ps->DepthBuffer));
    RenderDevice->SetupShader(resourcesManager.GetRealShader(ps->CS), EShaderType::COMPUTE_SHADER);
    RenderDevice->SetupShader(resourcesManager.GetRealShader(ps->PS), EShaderType::PIXEL_SHADER);
    RenderDevice->SetupShader(resourcesManager.GetRealShader(ps->VS), EShaderType::VERTEX_SHADER);
    RenderDevice->SetupShader(resourcesManager.GetRealShader(ps->DS), EShaderType::DOMAIN_SHADER);
    RenderDevice->SetupShader(resourcesManager.GetRealShader(ps->HS), EShaderType::HULL_SHADER);
    RenderDevice->SetupShader(resourcesManager.GetRealShader(ps->GS), EShaderType::GEOMETRY_SHADER);
    RenderDevice->SetupConstBuffers(constBuffers,ps->constBuffersNum);
    RenderDevice->SetupInputLayout(resourcesManager.GetRealInputLayout(ps->vertexDeclaration));
    
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
            auto& resource = resourcesManager.GetResource((Resource*)PullPointer());
            const ResourceUpdateData& params = PullData<ResourceUpdateData>();
            const void* data = PullPointer(params.dataSize);
            RenderDevice->SetResourceData(resource,params.dstSubresource,
                params.rect,data, params.srcRowPitch,params.srcDepthPitch);
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
            
        }
    }
    RenderDevice->Present();
    queueShift = 0;
    pipelinesQueueShift = 0;
    drawCallsQueueShift = 0;
    pipelinesQueue.clear();
    commandQueue.clear();
    dataQueue.clear();
}
