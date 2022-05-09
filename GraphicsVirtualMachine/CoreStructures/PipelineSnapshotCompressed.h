#pragma once
#include <cstdint>
#include "PipelineSnapshot.h"

namespace GVM {

struct PipelineSnapshotCompressed {
    ShaderDesc VS, PS, CS, GS, HS, DS;

    uint32_t            DrawCallsNum;

    EPrimitiveTopology  primitiveType;

    RasterizerStateDesc             rasterizerState;
    DepthStencilStateDesc           depthStencilState;
    InputAssemblerDeclarationDesc   vertexDeclaration;
    IResourceView*                  DepthBuffer;


    uint8_t             resourceCount;
    uint8_t             samplerCount;
    uint8_t             viewportCount;
    uint8_t             texturesCount;
    uint8_t             constBuffersCount;

    IAbstract* Data;
    

};

}
