#include "ModelsManager.h"

using namespace Renderer;

ModelsManager::ModelsManager(IRenderer* renderer):renderer(renderer) {}

void ModelsManager::RegisterModel(const ModelData& model, size_t id) {
	auto& pModel = models[id];
	if (pModel.vertexBuffer != nullptr)
		ReleaseModel(id);

	auto* vertexBuffer = new D3D11VertexBufferBinding();
	

	pModel.vertexBuffer = vertexBuffer;
	vertexBuffer->vertexBuffer = renderer->GenVertexBuffer(0, BufferUsage::BUFFERUSAGE_NONE, static_cast<int32_t>(sizeof(ModelVertex)*model.verticies.size()));
	vertexBuffer->vertexOffset = 0;
	vertexBuffer->vertexStride = sizeof(ModelVertex);
	renderer->SetVertexBufferData(vertexBuffer->vertexBuffer, 0, (void*)model.verticies.data(), model.verticies.size(), sizeof(ModelVertex), sizeof(ModelVertex), SetDataOptions::SETDATAOPTIONS_NONE);
		
	
	pModel.indexBuffer = renderer->GenIndexBuffer(0, BufferUsage::BUFFERUSAGE_WRITEONLY, model.indexes.size()*sizeof(uint16_t));
	renderer->SetIndexBufferData(pModel.indexBuffer, 0, (void*)model.indexes.data(), model.indexes.size()*sizeof(uint16_t), SetDataOptions::SETDATAOPTIONS_DISCARD);
	pModel.pt = model.pt;
	pModel.primitiveCount = model.primitiveCount;
	pModel.indexBufferElementSize = sizeof(uint16_t) * 8;
}

void ModelsManager::ReleaseModel(size_t id) {
	auto& pModel = models[id];
	if (pModel.vertexBuffer != nullptr) {
		renderer->AddDisposeIndexBuffer(pModel.indexBuffer);
		renderer->AddDisposeVertexBuffer(((D3D11VertexBufferBinding*)&pModel.vertexBuffer)->vertexBuffer);
		delete ((D3D11VertexBufferBinding*)&pModel.vertexBuffer);
		pModel.vertexBuffer = nullptr;
	}
}

ModelsManager::ModelCache ModelsManager::GetModel(size_t id) {
	return models[id];
}

ModelsManager::~ModelsManager() {
	for (auto& [key, model] : models) {
		ReleaseModel(key);
	}
}
