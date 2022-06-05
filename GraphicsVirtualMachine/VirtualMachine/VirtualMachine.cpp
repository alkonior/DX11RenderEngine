#include "VirtualMachine.h"

#include "RenderDeviceDX11/RenderDeviceDX11.h"

using namespace GVM;

void VirtualMachine::PushCommand(EMachineCommands command)
{
    commandStack.push_back(command);
}

void VirtualMachine::PushData(const void* data, uint32_t dataSize)
{
    uint32_t position = dataStack.size();
    dataStack.resize(position+dataSize);
    memcpy_s(dataStack.data()+position,dataSize,data,dataSize);
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

void VirtualMachine::PushPSC(PipelineSnapshot& pipelineSnapshot)
{
    auto position = pipelinesQueue.size();
    pipelinesQueue.resize(position + pipelineSnapshot.GetSize(iStructSizes));
    pipelineSnapshot.Compress(
        PipelineSnapshot::CompressArgs(reinterpret_cast<PSC*>(pipelinesQueue.data() + position), resourcesManager, iStructSizes));
    PushCommand(EMachineCommands::SETUP_PIPELINE);
}
constexpr VirtualMachine::EMachineCommands VirtualMachine::ToCommand(EDrawCallType drawCall)
{
    switch (drawCall)
    {
    case EDrawCallType::DISPATCH: return EMachineCommands::DISPATCH;
    case EDrawCallType::DISPATCH_INDIRECT: return EMachineCommands::DRAW_INDEXED;
    case EDrawCallType::DRAW: return EMachineCommands::DRAW;
    case EDrawCallType::DRAW_AUTO: return EMachineCommands::DRAW_AUTO;
    case EDrawCallType::DRAW_INDEXED: return EMachineCommands::DRAW_INDEXED;
    case EDrawCallType::DRAW_INDEXED_INSTANCED: return EMachineCommands::DRAW_INDEXED_INSTANCED;
    case EDrawCallType::DRAW_INDEXED_INSTANCED_INDIRECT: return EMachineCommands::DRAW_INDEXED_INSTANCED_INDIRECT;
    case EDrawCallType::DRAW_INSTANCED: return EMachineCommands::DRAW_INSTANCED;
    case EDrawCallType::DRAW_INSTANCED_INDIRECT: return EMachineCommands::DRAW_INSTANCED_INDIRECT;
    }
    return EMachineCommands::UNKNOWN;
}

void VirtualMachine::PushDrawCall(const DrawCall& drawCall)
{
    PushCommand(ToCommand(drawCall.type));
    PushData(&drawCall.args, sizeof(drawCall.args));
}
