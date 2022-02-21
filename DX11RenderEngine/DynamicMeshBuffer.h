#pragma once
#include "Renderer.h"
#include "UPRendererUtils.h"

struct MeshHashData {
	int vertexOffset = 0;
	int indexOffset = 0;
	int numElem = 0;
	Renderer::PrimitiveType pt;
};

class DynamicMeshBuffer {
	Renderer::IRenderer* renderer;

	size_t vertexBuffSize = 0;
	size_t indexBuffSize = 0;
	size_t vertexBuffCapacity = 20000;
	size_t indexBuffCapacity = 20000;


	std::vector<UPVertex> cpuVertices;
	std::vector<std::uint32_t> cpuIndexes;

public:

	Renderer::VertexBufferBinding vertexBuffer;
	Renderer::Buffer* indexBuffer = nullptr;

	DynamicMeshBuffer(Renderer::IRenderer* renderer, size_t vertexBuffCapacity, size_t indexBuffCapacity);

	void ResizeVertexBuffer(size_t newVertexBuffSize);
	void ResizeIndexBuffer(size_t newIndexBuffSize);

	void UpdateBuffers(bool force = false);

	MeshHashData AddMesh(const UPModelData& model);
	void UpdateMesh(const MeshHashData& odlModel, const UPModelData& newModel);

	void Flush();

	~DynamicMeshBuffer();
};

