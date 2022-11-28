#include "ModelsManager.h"

using namespace Renderer;


void ModelsManager::ReleaseModel(size_t id) {
	auto& pModel = models[id];
	if (pModel.vertexBuffer.buffersCount != 0) {
		VertexBufferBinding vB = pModel.vertexBuffer;
		renderDevice->AddDisposeIndexBuffer(pModel.indexBuffer);
		for (size_t i = 0; i < vB.buffersCount; i++) {
			renderDevice->AddDisposeVertexBuffer(vB.vertexBuffers[i]);
		}
		delete[] vB.vertexBuffers;
		delete[] vB.vertexOffset;
		delete[] vB.vertexStride;
		pModel.vertexBuffer.buffersCount = 0;

	}
}

ModelsManager::SavedModel ModelsManager::GetModel(size_t id) {
	return models[id];
}

ModelsManager::~ModelsManager() {
	for (auto& [key, model] : models) {
		ReleaseModel(key);
	}
}


void ModelsManager::RegisterModel(size_t id, const ModelMesh& model)
{
	UniversalRegisterModel(id, model);
}

void ModelsManager::RegisterModel(size_t id, const OpaqueMesh& model)
{
	UniversalRegisterModel(id, model);
}

bool ModelsManager::WasIdUsed(size_t id)
{
	return models[id].vertexBuffer.buffersCount != 0;
}
