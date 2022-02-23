#pragma once
#include "TransformUtils.h"
#include "DynamicMeshBuffer.h"

struct ParticleVertex {
	float3 position;
	float4 color;
};

typedef DynamicMeshBuffer<ParticleVertex>::MeshData ParticlesMesh;
