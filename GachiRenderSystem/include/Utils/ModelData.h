#pragma once
#include <vector>
#include "Utils/VertexesData.h"

enum EPrimitiveType : uint8_t {
	PRIMITIVETYPE_TRIANGLELIST = 0,
	PRIMITIVETYPE_TRIANGLESTRIP,
	PRIMITIVETYPE_LINELIST,
	PRIMITIVETYPE_LINESTRIP,
	PRIMITIVETYPE_POINTLIST_EXT,
	PRIMITIVETYPE_UNKNOWN
};

template<class VertexType>
struct MeshData {
	EPrimitiveType pt;
	size_t primitiveCount;
	std::vector<VertexType> vertixes;
	std::vector<uint32_t> indexes;
};


class ModelData {
public:
	std::vector<ModelVertex> verticies;
	std::vector<uint16_t> indexes;
	EPrimitiveType pt;
	size_t primitiveCount;
	ModelData(std::vector<ModelVertex> verticies, std::vector<uint16_t> indexes, EPrimitiveType pt, size_t primitiveCount);

};

typedef MeshData<ParticleVertex> ParticlesMesh;

typedef  MeshData<UPVertex> UPModelMesh;