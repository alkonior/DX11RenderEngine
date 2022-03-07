#pragma once
#include "Utils/TransformUtils.h"
#include "Utils/DynamicMeshBuffer.h"

struct UPVertex {
	float3 position;
	float2 texcoord;
	float2 lightTexcoord;
};

typedef DynamicMeshBuffer<UPVertex>::MeshData UPModelData;
