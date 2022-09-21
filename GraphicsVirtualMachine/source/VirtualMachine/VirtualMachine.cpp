#include "VirtualMachine.h"


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

VirtualMachine::VirtualMachine(IRenderDevice* RenderDevice):
resourcesManager(), RenderDevice(RenderDevice)
{}


void VirtualMachine::PushPSC(const PipelineSnapshot& pipelineSnapshot)
{
    auto position = pipelinesQueue.size();
    auto psSize = pipelineSnapshot.GetSize();
    pipelinesQueue.resize(position + psSize);
    pipelineSnapshot.Compress(
        PipelineSnapshot::CompressArgs(reinterpret_cast<PSC*>(pipelinesQueue.data() + position), resourcesManager));
    PushCommand(EMachineCommands::SETUP_PIPELINE);
   
    
}

void VirtualMachine::PushDrawCall(const DrawCall& drawCall)
{
    PushCommand(EMachineCommands::DRAW);
    drawCallsQueue.push_back(drawCall);
}
