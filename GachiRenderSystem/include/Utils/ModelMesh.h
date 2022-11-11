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
	std::vector<VertexType> vertices;
	std::vector<uint32_t> indexes;
};



typedef MeshData<ModelVertex> ModelMesh;
typedef MeshData<OpaqueModelVertex> OpaqueMesh;
typedef MeshData<ParticleVertex> ParticlesMesh;

typedef  MeshData<UPVertex> UPModelMesh;


typedef size_t StaticMeshId;
typedef size_t OpaqueMeshId;