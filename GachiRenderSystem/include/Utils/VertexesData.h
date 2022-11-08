#pragma once
#include "TransformUtils.h"


struct UPVertex {
    float3 position;
    float3 normal;
    float2 texcoord;
    float2 lightTexcoord;
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

struct DebugVertex3D {
    float3 position;
    //float3 color;
};

struct DebugVertex2D {
    float2 position;
    //float3 color;
};

struct FramedModelCommon {
    float2 texcoord;
};

struct FramedModelVertex {
    float3 position;
    float3 normal;
};

struct OpaqueModelVertex {
    float3 position;
    float3 normal;
    float3 binormal;
    float3 tangent;
    color color;
    float2 texcoord;
};