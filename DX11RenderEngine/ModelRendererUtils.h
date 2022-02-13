#pragma once
#include "TransformUtils.h"

struct ModelVertex {
    float3 position;
    float3 normal;
    float2 texcoord;
};

struct FramedModelVertex {
    float3 normal;
    float2 texcoord;
};

typedef std::vector<float3> Frame;

#pragma pack(push, 4)
struct Shader3DTranformCosntBuffer {
    matrix world;
    matrix view;
    matrix projection;
};

struct Shader3DDataCosntBuffer {
    float alpha;
    float w;
    float h;
};
#pragma pack(pop)