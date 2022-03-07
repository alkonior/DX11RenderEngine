#pragma once
#include "Utils/TransformUtils.h"
#include "Utils/DynamicMeshBuffer.h"

struct ParticleVertex {
	float3 position;
	float4 color;
};

typedef DynamicMeshBuffer<ParticleVertex>::MeshData ParticlesMesh;
