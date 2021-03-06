#pragma once
#include "IRenderer/IRenderer.h"
#include "TransformUtils.h"

struct MeshHashData {
	int vertexOffset = 0;
	int indexOffset = 0;
	int numElem = 0;
	Renderer::PrimitiveType pt;
};

template<class VertexType>
class DynamicMeshBuffer {

	size_t vertexBuffSize = 0;
	size_t indexBuffSize = 0;
	size_t vertexBuffCapacity = 20000;
	size_t indexBuffCapacity = 20000;


	std::vector<VertexType> cpuVertices;
	std::vector<std::uint32_t> cpuIndexes;


	void ResizeVertexBuffer(size_t newVertexBuffSize) {
		auto buffVB = mesh.vertexBuffer.vertexBuffers[0];
		mesh.vertexBuffer.vertexBuffers[0] = renderer->GenVertexBuffer(1, Renderer::BufferUsage::BUFFERUSAGE_WRITEONLY, newVertexBuffSize * sizeof(VertexType));
		renderer->AddDisposeVertexBuffer(buffVB);
		vertexBuffCapacity = newVertexBuffSize;
		vertexBuffSize = 0;
	}

	void ResizeIndexBuffer(size_t newIndexBuffSize) {
		auto buffIB = mesh.indexBuffer;
		mesh.indexBuffer = renderer->GenIndexBuffer(1, Renderer::BufferUsage::BUFFERUSAGE_WRITEONLY, newIndexBuffSize * sizeof(std::uint32_t));
		renderer->AddDisposeIndexBuffer(buffIB);
		indexBuffCapacity = newIndexBuffSize;
		indexBuffSize = 0;
	}
public:
	Renderer::MeshBindings mesh;
	//Renderer::VertexBufferBinding vertexBuffer;
	//Renderer::Buffer* indexBuffer = nullptr;


	struct MeshData {
		Renderer::PrimitiveType pt;
		size_t primitiveCount;
		std::vector<VertexType> vertixes;
		std::vector<uint32_t> indexes;
	};


	DynamicMeshBuffer(size_t vertexBuffCapacity, size_t indexBuffCapacity) : vertexBuffCapacity(vertexBuffCapacity), indexBuffCapacity(indexBuffCapacity) {
		mesh.vertexBuffer.buffersCount = 1;
		mesh.vertexBuffer.vertexBuffers = new Renderer::Buffer * [1]();
		mesh.vertexBuffer.vertexBuffers[0] = renderer->GenVertexBuffer(1, Renderer::BufferUsage::BUFFERUSAGE_WRITEONLY, sizeof(VertexType) * vertexBuffCapacity);
		mesh.vertexBuffer.vertexOffset = new (UINT)(0);
		mesh.vertexBuffer.vertexStride = new (UINT)(sizeof(VertexType));

		mesh.indexSize = 32;

		mesh.indexBuffer = renderer->GenIndexBuffer(1, Renderer::BufferUsage::BUFFERUSAGE_WRITEONLY, sizeof(uint32_t) * indexBuffCapacity);

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
			renderer->SetVertexBufferData(mesh.vertexBuffer.vertexBuffers[0], 0, cpuVertices.data(), cpuVertices.size(), sizeof(VertexType), sizeof(VertexType), Renderer::SetDataOptions::SETDATAOPTIONS_DISCARD);
			vertexBuffSize = cpuVertices.size();
		}

		if (indexBuffSize != cpuIndexes.size() || force) {
			renderer->SetIndexBufferData(mesh.indexBuffer, 0, cpuIndexes.data(), cpuIndexes.size() * sizeof(std::uint32_t), Renderer::SetDataOptions::SETDATAOPTIONS_DISCARD);
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

	//bool forceUpdate = true;

	void UpdateMesh(const MeshHashData& oldModel, const MeshData& newModel) {

		for (size_t i = 0; i < newModel.indexes.size(); i++) {
			cpuIndexes[oldModel.indexOffset + i] = (newModel.indexes[i] + oldModel.vertexOffset);
		}

		for (size_t i = 0; i < newModel.vertixes.size(); i++) {
			cpuVertices[oldModel.vertexOffset + i] = (newModel.vertixes[i]);
		}
	}

	void Flush() {
		cpuIndexes.clear();
		cpuVertices.clear();


		vertexBuffSize = 0;
		indexBuffSize = 0;
	};


	~DynamicMeshBuffer() {
		delete  mesh.vertexBuffer.vertexOffset;
		delete  mesh.vertexBuffer.vertexStride;
		renderer->AddDisposeVertexBuffer(mesh.vertexBuffer.vertexBuffers[0]);
		renderer->AddDisposeIndexBuffer(mesh.indexBuffer);
		delete[] mesh.vertexBuffer.vertexBuffers;
	}
};

