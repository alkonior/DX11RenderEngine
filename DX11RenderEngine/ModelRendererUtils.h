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
    float4 color;
    float alpha;
    float w;
    float h;
};

struct Shader3DDataCosntBuffer {
};
#pragma pack(pop)