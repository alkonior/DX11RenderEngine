#ifdef __cplusplus
#pragma once
#include "Utils/TransformUtils.h"
#include "CoreShaderInclude.h"
#endif


struct UPCosntBuffer {
    matrix world;
    float4 color;
    float2 texOffset;
};

STRUCTURE(1, UPCosntBuffer, upCosntBuffer)