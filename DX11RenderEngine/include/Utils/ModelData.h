#pragma once
#include "Utils/VertexesData.h"

enum PrimitiveType {
	PRIMITIVETYPE_TRIANGLELIST,
	PRIMITIVETYPE_TRIANGLESTRIP,
	PRIMITIVETYPE_LINELIST,
	PRIMITIVETYPE_LINESTRIP,
	PRIMITIVETYPE_POINTLIST_EXT,
	PRIMITIVETYPE_UNKNOWN
};

template<class VertexType>
struct MeshData {
	PrimitiveType pt;
	size_t primitiveCount;
	std::vector<VertexType> vertixes;
	std::vector<uint32_t> indexes;
};


class ModelData {
public:
	std::vector<ModelVertex> verticies;
	std::vector<uint16_t> indexes;
	PrimitiveType pt;
	size_t primitiveCount;
	ModelData(std::vector<ModelVertex> verticies, std::vector<uint16_t> indexes, PrimitiveType pt, size_t primitiveCount);

};

typedef std::vector<FramedModelVertex> Frame;

class FramedModelData {
public:
	std::vector<FramedModelCommon> verticies;
	std::vector<Frame> frames;
	std::vector<uint16_t> indexes;
	PrimitiveType pt;
	size_t primitiveCount;

	FramedModelData();
	FramedModelData(std::vector<FramedModelCommon> verticies, std::vector<Frame> frames, std::vector<uint16_t> indexes, PrimitiveType pt, size_t primitiveCount);


	void AddTriangle(const std::vector<Frame>& verticies3, const FramedModelCommon uvs[3]);

};

typedef MeshData<ParticleVertex> ParticlesMesh;

typedef  MeshData<UPVertex> UPModelMesh;