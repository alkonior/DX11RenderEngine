#include "VirtualMachine.h"

#include "RenderDeviceDX11/RenderDeviceDX11.h"

using namespace GVM;

VirtualMachine::VirtualMachine(const RenderDeviceInitParams& initParams, bool debugMode)
{
    switch (initParams.device)
    {
    case EDeviceToUse::DX11:
        {
            RenderDevice = new RenderDeviceDX11(initParams, debugMode);
            break;
        }
    default:
        {
            break;
        }
    }
}

void VirtualMachine::PushPSC(PipelineSnapshot& pipelineSnapshot)
{
    auto position = pipelinesQueue.size();
    pipelinesQueue.resize(position + pipelineSnapshot.GetSize(iStructSizes));
    pipelineSnapshot.Compress(
        PipelineSnapshot::CompressArgs(reinterpret_cast<PSC*>(pipelinesQueue.data() + position), resourcesManager, iStructSizes));
}

void VirtualMachine::PushDrawCall(DrawCall drawCall)
{}
