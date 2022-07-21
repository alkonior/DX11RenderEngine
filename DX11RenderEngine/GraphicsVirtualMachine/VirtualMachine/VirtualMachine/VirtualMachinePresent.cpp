#include "VirtualMachine.h"

#include "GraphicsVirtualMachine/RenderDeviceDX11/RenderDeviceDX11.h"


using namespace GVM;

void VirtualMachine::ExecuteSetupPipeline()
{
    
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
            
            break;
        }
            
        }
    }
    queueShift = 0;
    pipelinesQueue.clear();
    commandQueue.clear();
    dataQueue.clear();
}
