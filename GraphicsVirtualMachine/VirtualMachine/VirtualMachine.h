#pragma once
#include "RenderDevice.h"
#include "CoreStructures\PipelineSnapshotCompressed.h"

namespace GVM {

template<class T>
using VMStack =  std::vector<T>;

typedef typename PipelineSnapshotCompressed PSC;

class VirtualMachine {
    VMStack<uint32_t> intStack;
    VMStack<EDrawCallType> drawCallsStack;
    VMStack<uint32_t> dataStack;
    VMStack<PSC*> pipelinesQueue;

public:

    void PushDrawCall(EDrawCallType type, uint32_t args[5]);
    void PushPSC(PSC* psc);
    uint32_t PushData(void* data, uint32_t dataLength);




    
};

}