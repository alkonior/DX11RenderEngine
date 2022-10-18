
#ifndef HLSL
#pragma once
#include "TransformUtils.h"
#include "CoreRenderSystem/CoreShaderInclude.h"
#endif

struct UIRenderPassConstBuffer {
    matrix transform;
    float2 uvShift;
    float2 uvScale;
    float4 color;
};



STRUCTURE(0, UIRenderPassConstBuffer, uiTransformCB)