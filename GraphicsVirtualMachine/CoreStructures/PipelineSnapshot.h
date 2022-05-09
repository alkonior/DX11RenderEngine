#pragma once
#include <vector>

#include "VirtualMachine\IRenderClasses.h"
#include "BaseStructures.h"


namespace GVM {

struct PipelineSnapshot {

    IShader* VS, PS, CS, GS, HS, DS;

    uint32_t DrawCallsNum;

    EPrimitiveTopology primitiveType;

    IRasterizerState* rasterizerState;
    IDepthStencilState* depthStencilState;
    IInputLayout* vertexDeclaration;

    
    uint8_t constBuffersCount;
    IResourceView* DepthBuffer;
    
    

};


}
