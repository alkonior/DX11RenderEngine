#include "DynamicMeshBuffer.h"

using namespace Renderer;

DynamicMeshBuffer::DynamicMeshBuffer(Renderer::IRenderer* renderer, size_t vertexBuffCapacity, size_t indexBuffCapacity) :
	renderer(renderer), vertexBuffCapacity(vertexBuffCapacity), indexBuffCapacity(indexBuffCapacity) {
	
	vertexBuffer.buffersCount = 1;
	vertexBuffer.vertexBuffers = new Buffer * [1]();
	vertexBuffer.vertexBuffers[0] = renderer->GenVertexBuffer(1, BufferUsage::BUFFERUSAGE_WRITEONLY, sizeof(UPVertex) * vertexBuffCapacity);
	vertexBuffer.vertexOffset = new (UINT)(0);
	vertexBuffer.vertexStride = new (UINT)(sizeof(UPVertex));
	
	
	
	indexBuffer = renderer->GenIndexBuffer(1, BufferUsage::BUFFERUSAGE_WRITEONLY, sizeof(uint32_t) * indexBuffCapacity);

	cpuVertices.reserve(vertexBuffCapacity);
	cpuIndexes.reserve(indexBuffCapacity);
}

void DynamicMeshBuffer::ResizeVertexBuffer(size_t newVertexBuffSize) {
	auto buffVB = vertexBuffer.vertexBuffers[0];
	vertexBuffer.vertexBuffers[0] = renderer->GenVertexBuffer(1, BufferUsage::BUFFERUSAGE_WRITEONLY, newVertexBuffSize * sizeof(UPVertex));
	renderer->AddDisposeVertexBuffer(buffVB);
	vertexBuffCapacity = newVertexBuffSize;
	vertexBuffSize = 0;
}

void DynamicMeshBuffer::ResizeIndexBuffer(size_t newIndexBuffSize) {
	auto buffIB = indexBuffer;
	indexBuffer = renderer->GenIndexBuffer(1, BufferUsage::BUFFERUSAGE_WRITEONLY, newIndexBuffSize * sizeof(std::uint32_t));
	renderer->AddDisposeIndexBuffer(buffIB);
	indexBuffCapacity = newIndexBuffSize;
	indexBuffSize = 0;
}

void DynamicMeshBuffer::UpdateBuffers(bool force) {
	if (vertexBuffCapacity <= cpuVertices.size()) {
		vertexBuffCapacity = vertexBuffSize;
		ResizeVertexBuffer(std::floor(vertexBuffCapacity * 1.3));
		//	renderer->SetVertexBufferData(bigVertexBuffer.vertexBuffers[0], 0, cpuVerticies.data(), cpuVerticies.size(), sizeof(UPVertex), sizeof(UPVertex), Renderer::SetDataOptions::SETDATAOPTIONS_DISCARD);
	}
	if (indexBuffCapacity <= cpuIndexes.size()) {
		indexBuffCapacity = indexBuffSize;
		ResizeIndexBuffer(std::floor(indexBuffCapacity * 1.3));
		//	renderer->SetIndexBufferData(bigIndexBuffer, 0, cpuIndexes.data(), cpuIndexes.size() * sizeof(std::uint32_t), Renderer::SetDataOptions::SETDATAOPTIONS_DISCARD);
	}	

	if (vertexBuffSize != cpuVertices.size() || force) {
		renderer->SetVertexBufferData(vertexBuffer.vertexBuffers[0], 0, cpuVertices.data(), cpuVertices.size(), sizeof(UPVertex), sizeof(UPVertex), Renderer::SetDataOptions::SETDATAOPTIONS_DISCARD);
		vertexBuffSize = cpuVertices.size();
	}

	if (indexBuffSize != cpuIndexes.size() || force) {
		renderer->SetIndexBufferData(indexBuffer, 0, cpuIndexes.data(), cpuIndexes.size() * sizeof(std::uint32_t), Renderer::SetDataOptions::SETDATAOPTIONS_DISCARD);
		indexBuffSize = cpuIndexes.size();
	}
}

MeshHashData DynamicMeshBuffer::AddMesh(const UPModelData& model) {

	MeshHashData res{ cpuVertices.size(), cpuIndexes.size(),  model.primitiveCount, model.pt };

	for (size_t i = 0; i < model.indexes.size(); i++) {
		cpuIndexes.push_back(model.indexes[i] + cpuVertices.size());
	}

	for (size_t i = 0; i < model.verticies.size(); i++) {
		cpuVertices.push_back(model.verticies[i]);
	}

	return res;
}

void DynamicMeshBuffer::UpdateMesh(const MeshHashData& odlModel, const UPModelData& newModel) {

	for (size_t i = 0; i < newModel.indexes.size(); i++) {
		cpuIndexes[odlModel.indexOffset + i] = (newModel.indexes[i] + odlModel.vertexOffset);
	}

	for (size_t i = 0; i < newModel.verticies.size(); i++) {
		cpuVertices[odlModel.vertexOffset + i] = (newModel.verticies[i]);
	}
}
void DynamicMeshBuffer::Flush() {
	cpuIndexes.clear();
	cpuVertices.clear();


	vertexBuffSize = 0;
	indexBuffSize = 0;
};


DynamicMeshBuffer::~DynamicMeshBuffer() {
	delete vertexBuffer.vertexOffset;
	delete vertexBuffer.vertexStride;
	renderer->AddDisposeVertexBuffer(vertexBuffer.vertexBuffers[0]);
	renderer->AddDisposeIndexBuffer(indexBuffer);
	delete[] vertexBuffer.vertexBuffers;
}
