#pragma once
#include "TransformUtils.h"

struct UIRenderPassConstBuffer {
    matrix transform;
    float2 uvShift;
    float2 uvScale;
    float4 color;
};
