#include "VirtualMachine.h"

using namespace GVM;

void VirtualMachine::PushPSC(PipelineSnapshot& pipelineSnapshot)
{
    auto position = pipelinesQueue.size();
    pipelinesQueue.resize(position + pipelineSnapshot.GetSize(iStructSizes));
    pipelineSnapshot.Compress(
        PipelineSnapshot::CompressArgs(reinterpret_cast<PSC*>(pipelinesQueue.data()+position), resourcesManager, iStructSizes));
    
    
}