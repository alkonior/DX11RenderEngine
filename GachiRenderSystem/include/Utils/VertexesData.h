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
    OpaqueModelVertex() = default;
    OpaqueModelVertex(float3 position): position(position){}
    OpaqueModelVertex(float3 position, float3 normal): position(position), normal(normal){}
    OpaqueModelVertex(float3 position, color color): position(position), color(color) {}
    OpaqueModelVertex(float3 position, float2 texcoord): position(position), texcoord(texcoord){}
    OpaqueModelVertex(float3 position, float3 normal, float2 texcoord): position(position), normal(normal), texcoord(texcoord){}
    OpaqueModelVertex(float3 position, color color, float2 texcoord): position(position), color(color), texcoord(texcoord){}

    
    float3 position;
    float3 normal;
    float2 texcoord;
    float3 binormal;
    float3 tangent;
    color  color;
};