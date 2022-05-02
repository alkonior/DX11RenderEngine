#pragma once
#include <cstdint>
#include "BaseDefines.h"
#include "BaseStructures\GVMEnums.h"

namespace GVM {

struct PipelineSnapshotCompressed {
    pointer vs, ps, cs, gs, hs, ds;

    byte DrawFlags;
    uint DrawArgs[5];

    EPrimitiveTopology primitiveType;

    pointer rasterizerState;
    pointer depthStencilState;
    pointer vertexDeclaration;

    byte resourceCount;
    byte samplerCount;
    byte viewportCount;
    byte texturesCount;
    byte constBuffersCount;

    pointer DepthBuffer;
    

};

}
