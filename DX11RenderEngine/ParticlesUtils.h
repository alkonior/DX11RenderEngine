#pragma once
#include "TransformUtils.h"
#include "DynamicMeshBuffer.h"

struct ParticleVertex {
	float3 position;
};

typedef DynamicMeshBuffer<ParticleVertex>::MeshData ParticlesMesh;
