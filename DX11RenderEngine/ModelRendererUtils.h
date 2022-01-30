#pragma once
#include "Transform.h"

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