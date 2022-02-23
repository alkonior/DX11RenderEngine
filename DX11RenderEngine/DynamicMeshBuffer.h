#pragma once
#include "Renderer.h"
#include "TransformUtils.h"

struct MeshHashData {
	int vertexOffset = 0;
	int indexOffset = 0;
	int numElem = 0;
	Renderer::PrimitiveType pt;
};

template<class VertexType>
class DynamicMeshBuffer {


	Renderer::IRenderer* renderer;

	size_t vertexBuffSize = 0;
	size_t indexBuffSize = 0;
	size_t vertexBuffCapacity = 20000;
	size_t indexBuffCapacity = 20000;


	std::vector<VertexType> cpuVertices;
	std::vector<std::uint32_t> cpuIndexes;


	void ResizeVertexBuffer(size_t newVertexBuffSize) {
		auto buffVB = vertexBuffer.vertexBuffers[0];
		vertexBuffer.vertexBuffers[0] = renderer->GenVertexBuffer(1, Renderer::BufferUsage::BUFFERUSAGE_WRITEONLY, newVertexBuffSize * sizeof(VertexType));
		renderer->AddDisposeVertexBuffer(buffVB);
		vertexBuffCapacity = newVertexBuffSize;
		vertexBuffSize = 0;
	}

	void ResizeIndexBuffer(size_t newIndexBuffSize) {
		auto buffIB = indexBuffer;
		indexBuffer = renderer->GenIndexBuffer(1, Renderer::BufferUsage::BUFFERUSAGE_WRITEONLY, newIndexBuffSize * sizeof(std::uint32_t));
		renderer->AddDisposeIndexBuffer(buffIB);
		indexBuffCapacity = newIndexBuffSize;
		indexBuffSize = 0;
	}
public:

	Renderer::VertexBufferBinding vertexBuffer;
	Renderer::Buffer* indexBuffer = nullptr;


	struct MeshData {
		Renderer::PrimitiveType pt;
		size_t primitiveCount;
		std::vector<VertexType> vertixes;
		std::vector<uint32_t> indexes;
	};


	DynamicMeshBuffer(Renderer::IRenderer* renderer, size_t vertexBuffCapacity, size_t indexBuffCapacity) :
		renderer(renderer), vertexBuffCapacity(vertexBuffCapacity), indexBuffCapacity(indexBuffCapacity) {

		vertexBuffer.buffersCount = 1;
		vertexBuffer.vertexBuffers = new Renderer::Buffer * [1]();
		vertexBuffer.vertexBuffers[0] = renderer->GenVertexBuffer(1, Renderer::BufferUsage::BUFFERUSAGE_WRITEONLY, sizeof(VertexType) * vertexBuffCapacity);
		vertexBuffer.vertexOffset = new (UINT)(0);
		vertexBuffer.vertexStride = new (UINT)(sizeof(VertexType));



		indexBuffer = renderer->GenIndexBuffer(1, Renderer::BufferUsage::BUFFERUSAGE_WRITEONLY, sizeof(uint32_t) * indexBuffCapacity);

		cpuVertices.reserve(vertexBuffCapacity);
		cpuIndexes.reserve(indexBuffCapacity);
	}



	void UpdateBuffers(bool force = false) {
		if (vertexBuffCapacity <= cpuVertices.size()) {
			vertexBuffCapacity = std::floor(cpuVertices.size() * 1.3);;
			ResizeVertexBuffer(vertexBuffCapacity);
			//	renderer->SetVertexBufferData(bigVertexBuffer.vertexBuffers[0], 0, cpuVerticies.data(), cpuVerticies.size(), sizeof(UPVertex), sizeof(UPVertex), Renderer::SetDataOptions::SETDATAOPTIONS_DISCARD);
		}
		if (indexBuffCapacity <= cpuIndexes.size()) {
			indexBuffCapacity = std::floor(cpuIndexes.size() * 1.3);
			ResizeIndexBuffer(indexBuffCapacity);
			//	renderer->SetIndexBufferData(bigIndexBuffer, 0, cpuIndexes.data(), cpuIndexes.size() * sizeof(std::uint32_t), Renderer::SetDataOptions::SETDATAOPTIONS_DISCARD);
		}

		if (vertexBuffSize != cpuVertices.size() || force) {
			renderer->SetVertexBufferData(vertexBuffer.vertexBuffers[0], 0, cpuVertices.data(), cpuVertices.size(), sizeof(VertexType), sizeof(VertexType), Renderer::SetDataOptions::SETDATAOPTIONS_DISCARD);
			vertexBuffSize = cpuVertices.size();
		}

		if (indexBuffSize != cpuIndexes.size() || force) {
			renderer->SetIndexBufferData(indexBuffer, 0, cpuIndexes.data(), cpuIndexes.size() * sizeof(std::uint32_t), Renderer::SetDataOptions::SETDATAOPTIONS_DISCARD);
			indexBuffSize = cpuIndexes.size();
		}
	}

	MeshHashData AddMesh(const MeshData& model) {

		MeshHashData res{ cpuVertices.size(), cpuIndexes.size(),  model.primitiveCount, model.pt };

		for (size_t i = 0; i < model.indexes.size(); i++) {
			cpuIndexes.push_back(model.indexes[i] + cpuVertices.size());
		}

		for (size_t i = 0; i < model.vertixes.size(); i++) {
			cpuVertices.push_back(model.vertixes[i]);
		}

		return res;
	}
	void UpdateMesh(const MeshHashData& odlModel, const MeshData& newModel) {

		for (size_t i = 0; i < newModel.indexes.size(); i++) {
			cpuIndexes[odlModel.indexOffset + i] = (newModel.indexes[i] + odlModel.vertexOffset);
		}

		for (size_t i = 0; i < newModel.vertixes.size(); i++) {
			cpuVertices[odlModel.vertexOffset + i] = (newModel.vertixes[i]);
		}
	}

	void Flush() {
		cpuIndexes.clear();
		cpuVertices.clear();


		vertexBuffSize = 0;
		indexBuffSize = 0;
	};


	~DynamicMeshBuffer() {
		delete vertexBuffer.vertexOffset;
		delete vertexBuffer.vertexStride;
		renderer->AddDisposeVertexBuffer(vertexBuffer.vertexBuffers[0]);
		renderer->AddDisposeIndexBuffer(indexBuffer);
		delete[] vertexBuffer.vertexBuffers;
	}
};

