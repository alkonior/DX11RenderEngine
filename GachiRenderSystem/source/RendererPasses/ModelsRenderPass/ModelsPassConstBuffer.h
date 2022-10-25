#ifdef __cplusplus
#pragma once
#include "TransformUtils.h"
#include "CoreRenderSystem/CoreShaderInclude.h"
#endif

struct ModelsCosntBuffer {
    matrix world;
    matrix oldWorld;
    float4 color;
    float alpha;
    float2 wh;
    float blurSwitch;
};

STRUCTURE(1, ModelsCosntBuffer, modelsCosntBuffer)