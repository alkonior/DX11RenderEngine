#include "VirtualMachine.h"

#include "GraphicsVirtualMachine/RenderDeviceDX11/RenderDeviceDX11.h"


using namespace GVM;


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
            
        }
    }
    pipelinesQueue.clear();
    commandQueue.clear();
    dataQueue.clear();
}
