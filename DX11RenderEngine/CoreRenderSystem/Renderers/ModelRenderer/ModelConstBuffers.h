#ifdef __cplusplus
#pragma once
#include "Utils/TransformUtils.h"
#include "CoreShaderInclude.h"
#endif



struct ModelsCosntBuffer {
    matrix oldWorld;
    matrix world;
    float4 color;
    float oldAlpha;
    float alpha;
    float2 wh;
    float blurSwitch;
};

STRUCTURE(1, ModelsCosntBuffer, modelsCosntBuffer)