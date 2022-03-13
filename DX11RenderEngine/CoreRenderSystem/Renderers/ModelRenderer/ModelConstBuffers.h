#ifdef __cplusplus
#pragma once
#include "Utils/TransformUtils.h"
#include "CoreShaderInclude.h"
#endif



struct ModelsCosntBuffer {
    matrix world;
    float4 color;
    float alpha;
    float2 wh;
};

STRUCTURE(1, ModelsCosntBuffer, modelsCosntBuffer)