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
    std::vector<uint16_t> indexes;
};


#pragma pack(push, 4)
struct UPTranformCosntBuffer {
    matrix world;
    matrix view;
    matrix projection;
    float4 color;
};

struct UPDataCosntBuffer {
};
#pragma pack(pop)