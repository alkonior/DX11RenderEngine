#pragma once
#include "TransformUtils.h"

struct UPVertex {
    float3 position;
    float2 texcoord;
    float2 lightTexcoord;
};

struct UPModelData {
    Renderer::PrimitiveType pt;
    size_t primitiveCount;
    std::vector<UPVertex> verticies;
    std::vector<uint32_t> indexes;
};
