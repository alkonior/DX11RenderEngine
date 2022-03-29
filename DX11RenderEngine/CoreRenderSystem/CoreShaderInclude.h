#ifndef HLSL
#pragma once
#include "Utils/TransformUtils.h"
// COMMON


struct CONST_HANDLE {
	CONST_HANDLE(int slot) { this->slot = slot; }
	int slot;
};

struct TEXTURE_HANDLE {
	TEXTURE_HANDLE(int slot) { this->slot = slot; }
	int slot;
};
#endif

#ifdef HLSL
#define STRUCTURE(slot, typeName, name) cbuffer __buffer##slot : register(b##slot) { typeName name : packoffset(c0); }
#else
#define STRUCTURE(slot, typeName, name) const CONST_HANDLE name(slot);
#endif

#ifdef HLSL
#define TEXTURE_2D(slot, type, name) Texture2D<type> name : register(t##slot);
#else
#define TEXTURE_2D(slot, type, typeName, name) const TEXTURE_HANDLE  name(slot);
#endif


struct MainConstants {
	
	matrix view;
	matrix projection;
	matrix inverseView;
	matrix inverseProjection;
	
	matrix past_view;
	matrix past_projection;
	matrix past_inverseView;
	matrix past_inverseProjection;


	matrix viewProjection;
	matrix past_viewProjection;
	matrix viewProjectionInverse;
	matrix past_viewProjectionInverse;
	
	matrix reprojectionMatrix;
	
	
	float  dt;
};

struct TAAShiftBuffer {
	float2  taaPixelShift;
	float  taaStrength;
};


STRUCTURE(0, MainConstants, mainConstants)
STRUCTURE(8, TAAShiftBuffer, taaShiftBuffer)



/*
// HEADER

struct CAMERA {
	float4x4 view;
	float4x4 proj;
	float3   pos;
}

struct MODEL {
	float4 world;
}

STRUCTURE(0, CAMERA, camera)
STRUCTURE(1, MODEL, model)
TEXTURE_2D(0, float4, colorTexture)

// HLSL

p = mul(camera.view, v);
c = colorTexture.Sample(...);

// C++

CAMERA camera = /* .../
constBuffer->SetData(camera);
SetConstBuffer(camera.slot, constBuffer);

SetShaderResource(colorTexture.slot, /* ... /);

*/