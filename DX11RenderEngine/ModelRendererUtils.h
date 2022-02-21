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
