#ifdef __cplusplus
#pragma once
#include "TransformUtils.h"
#include "CoreShaderInclude.h"
#endif


struct Shader3DTranformCosntBuffer {
    matrix world;
    matrix view;
    matrix projection;
};

struct Shader3DDataCosntBuffer {
    float4 color;
    float alpha;
    float2 wh;
};

STRUCTURE(0, Shader3DTranformCosntBuffer, ModelsTransform)
STRUCTURE(1, Shader3DDataCosntBuffer, ModelsExtraData)