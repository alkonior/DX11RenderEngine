﻿#pragma once
#include "TransformUtils.h"


struct UPVertex {
    float3 position;
    float3 normal;
    float2 texcoord;
    float2 lightTexcoord;
    float2 dynamicLightTexcoord;
};

struct ParticleVertex {
    float3 position;
    float4 color;
};


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