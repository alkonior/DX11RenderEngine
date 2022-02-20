#pragma once
#include "TransformUtils.h"

struct ModelVertex {
    float3 position;
    float3 normal;
    float2 texcoord;
};

struct FramedModelCommon {
    float2 texcoord;
};

struct FramedModelVertex {
    float3 position;
    float3 normal;
};

typedef std::vector<FramedModelVertex> Frame;

#pragma pack(push, 4)
struct Shader3DTranformCosntBuffer {
    matrix world;
    matrix view;
    matrix projection;
};

struct Shader3DDataCosntBuffer {
    float4 color;
    float alpha;
    float w;
    float h;
};
#pragma pack(pop)