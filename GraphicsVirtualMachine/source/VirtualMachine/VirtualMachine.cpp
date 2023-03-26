#include "VirtualMachine.h"


using namespace GVM;

VirtualMachine::~VirtualMachine()
{
    delete RenderDevice;
}



void VirtualMachine::PushData(const void* data, uint32_t dataSize)
{
    uint32_t position = dataQueue.size();
    dataQueue.resize(position + dataSize);
    memcpy_s(dataQueue.data() + position, dataSize, data, dataSize);
}

VirtualMachine::VirtualMachine(IRenderDevice* RenderDevice):
    resourcesManager(), RenderDevice(RenderDevice), renderGraph(RenderDevice)
{}


void VirtualMachine::PushPSC(const PipelineSnapshot& pipelineSnapshot)
{
    lastPSPosition = pipelinesQueue.size();
    auto psSize = pipelineSnapshot.GetSize();
    pipelinesQueue.resize(lastPSPosition + psSize);
    pipelineSnapshot.Compress(
        PipelineSnapshot::CompressArgs(reinterpret_cast<PSC*>(pipelinesQueue.data() + lastPSPosition), resourcesManager));
    pipelinesQueueShift += psSize;
    
    //PushCommand(EMachineCommands::SETUP_PIPELINE);
}

void VirtualMachine::PushDrawCall(const DrawCall& drawCall)
{
    drawCallsQueue.push_back({drawCall, lastPSPosition});
    PushCommand(EMachineCommands::DRAW);
}

void VirtualMachine::BeginEvent(const char* name)
{
    PushData(name, std::strlen(name) + 1);
    PushCommand(EMachineCommands::BEGIN_EVENT);
}

void VirtualMachine::EndEvent()
{
    PushCommand(EMachineCommands::END_EVENT);
}
