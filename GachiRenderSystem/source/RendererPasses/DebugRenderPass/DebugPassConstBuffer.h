#ifdef __cplusplus
#pragma once
#include "TransformUtils.h"
#include "CoreRenderSystem/CoreShaderInclude.h"
#endif

struct DebugCosntBuffer {
    matrix world;
    float3 color;
};

STRUCTURE(1, DebugCosntBuffer, debugCosntBuffer)