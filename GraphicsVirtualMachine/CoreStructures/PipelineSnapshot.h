#pragma once
#include <vector>
#include "BaseDefines.h"

#include "BaseStructures\BaseStructures.h"


namespace GVM {

struct PipelineSnapshot {

    Shader *vs, ps, cs, gs, hs, ds;

    byte DrawFlags;
    uint DrawArgs[5];

    EPrimitiveTopology primitiveType;

    RasterizerState* rasterizerState;
    DepthStencilState* depthStencilState;
    InputAssemblerDeclaration* vertexDeclaration;

    
    byte constBuffersCount;
    pointer DepthBuffer;
    
    pointer data;

};


}
