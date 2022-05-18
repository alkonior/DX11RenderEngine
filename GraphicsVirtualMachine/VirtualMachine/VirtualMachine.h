#pragma once
#include "RenderDevice.h"
#include "ResourcesManager.h"
#include "CoreStructures\PipelineSnapshot.h"

namespace GVM {

template<class T>
using VMStack =  std::vector<T>;

typedef typename Compressed::PipelineSnapshot PSC;

class VirtualMachine {
    VMStack<uint32_t> intStack;
    VMStack<EDrawCallType> drawCallsStack;
    VMStack<uint32_t> dataStack;
    VMStack<PSC*> pipelinesQueue;

public:
    ResourcesManager resourcesManager;

    VirtualMachine(const RenderDeviceInitParams& initParams, bool debugMode = true);
    VirtualMachine(const VirtualMachine&) = delete;
    VirtualMachine(const VirtualMachine&&) = delete;
    ~VirtualMachine() = default;


    void PushPSC(PipelineSnapshot& pipelineSnapshot);
    void PushDrawCall(DrawCall);
    
    uint32_t PushData(void* data, uint32_t dataLength);




    
};

}