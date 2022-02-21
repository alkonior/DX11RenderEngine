#ifndef HLSL
#pragma once

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