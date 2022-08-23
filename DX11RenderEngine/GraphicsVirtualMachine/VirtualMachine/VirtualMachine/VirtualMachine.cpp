#include "VirtualMachine.h"

#include "GraphicsVirtualMachine/RenderDeviceDX11/RenderDeviceDX11.h"


using namespace GVM;

void VirtualMachine::PushCommand(EMachineCommands command)
{
    commandQueue.push_back(command);
}

void VirtualMachine::PushData(const void* data, uint32_t dataSize)
{
    uint32_t position = dataQueue.size();
    dataQueue.resize(position+dataSize);
    memcpy_s(dataQueue.data()+position,dataSize,data,dataSize);
}

VirtualMachine::VirtualMachine(const RenderDeviceInitParams& initParams, bool debugMode):
resourcesManager()
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


void VirtualMachine::PushPSC(const PipelineSnapshot& pipelineSnapshot)
{
    auto position = pipelinesQueue.size();
    auto psSize = pipelineSnapshot.GetSize(iStructSizes);
    pipelinesQueue.resize(position + psSize);
    pipelineSnapshot.Compress(
        PipelineSnapshot::CompressArgs(reinterpret_cast<PSC*>(pipelinesQueue.data() + position), resourcesManager, iStructSizes));
    PushCommand(EMachineCommands::SETUP_PIPELINE);
   
    
}

void VirtualMachine::PushDrawCall(const DrawCall& drawCall)
{
    PushCommand(EMachineCommands::DRAW);
    drawCallsQueue.push_back(drawCall);
}
