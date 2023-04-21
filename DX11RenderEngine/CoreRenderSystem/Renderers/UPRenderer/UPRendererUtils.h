#pragma once
#include "Utils/TransformUtils.h"
#include "Utils/DynamicMeshBuffer.h"

struct UPVertex {
	float3 position;
	float3 normal;
	float2 texcoord;
	float2 lightTexcoord;
	float2 dynamicLightTexcoord;
};

typedef DynamicMeshBuffer<UPVertex>::MeshData UPModelData;
