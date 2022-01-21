#pragma once
#include "Transform.h"



struct ModelVertex {
    float3 position;
    float3 normal;
    float2 texcoord;

    static const D3D11_INPUT_ELEMENT_DESC  InputElements[ ];
};