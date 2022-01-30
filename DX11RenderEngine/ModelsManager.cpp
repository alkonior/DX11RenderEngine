#include "ModelsManager.h"

using namespace Renderer;

ModelsManager::ModelsManager(IRenderer* renderer) :renderer(renderer) {}

void ModelsManager::RegisterModel(const ModelData& model, size_t id) {
	auto& pModel = models[id];
	if (pModel.vertexBuffer != nullptr)
		ReleaseModel(id);

	auto* vertexBuffer = new VertexBufferBinding();


	pModel.vertexBuffer = vertexBuffer;
	vertexBuffer->vertexBuffers = new Buffer * [1];
	vertexBuffer->vertexBuffers[0] = renderer->GenVertexBuffer(0, BufferUsage::BUFFERUSAGE_NONE, static_cast<int32_t>(sizeof(ModelVertex) * model.verticies.size()));
	vertexBuffer->vertexOffset = new UINT(0);
	vertexBuffer->vertexStride = new UINT(sizeof(ModelVertex));
	renderer->SetVertexBufferData(vertexBuffer->vertexBuffers[0], 0, (void*)model.verticies.data(), (UINT)model.verticies.size(), (UINT)sizeof(ModelVertex), (UINT)sizeof(ModelVertex), SetDataOptions::SETDATAOPTIONS_NONE);


	pModel.indexBuffer = renderer->GenIndexBuffer(0, BufferUsage::BUFFERUSAGE_WRITEONLY, (UINT)(model.indexes.size() * sizeof(uint16_t)));
	renderer->SetIndexBufferData(pModel.indexBuffer, 0, (void*)model.indexes.data(), (UINT)(model.indexes.size() * sizeof(uint16_t)), SetDataOptions::SETDATAOPTIONS_DISCARD);
	pModel.pt = model.pt;
	pModel.primitiveCount = model.primitiveCount;
	pModel.indexBufferElementSize = sizeof(uint16_t) * 8;
}

void ModelsManager::RegisterFramedModel(const FramedModelData& model, size_t id) {
	auto& pModel = models[id];
	if (pModel.vertexBuffer != nullptr)
		ReleaseModel(id);

	auto* vertexBuffer = new VertexBufferBinding();


	pModel.vertexBuffer = vertexBuffer;
	const size_t framesCount = model.frames.size();
	pModel.vertexBuffer->buffersCount = framesCount + 1;
	vertexBuffer->vertexBuffers = new Buffer * [framesCount + 1];
	vertexBuffer->vertexOffset = new UINT[framesCount + 1];
	vertexBuffer->vertexStride = new UINT[framesCount + 1];

	vertexBuffer->vertexBuffers[0] = renderer->GenVertexBuffer(0, BufferUsage::BUFFERUSAGE_NONE, static_cast<int32_t>(sizeof(FramedModelVertex) * model.verticies.size()));
	vertexBuffer->vertexOffset[0] = UINT(0);
	vertexBuffer->vertexStride[0] = UINT(sizeof(FramedModelVertex));
	renderer->SetVertexBufferData(vertexBuffer->vertexBuffers[0], 0, (void*)model.verticies.data(), (UINT)model.verticies.size(), (UINT)sizeof(FramedModelVertex), (UINT)sizeof(FramedModelVertex), SetDataOptions::SETDATAOPTIONS_NONE);

	for (size_t i = 0; i < framesCount; i++) {
		vertexBuffer->vertexBuffers[i + 1] = renderer->GenVertexBuffer(0, BufferUsage::BUFFERUSAGE_NONE, static_cast<int32_t>(sizeof(float3) * model.frames[i].size()));
		vertexBuffer->vertexOffset[i + 1] = UINT(0);
		vertexBuffer->vertexStride[i + 1] = UINT(sizeof(float3));
		renderer->SetVertexBufferData(vertexBuffer->vertexBuffers[i + 1], 0, (void*)model.frames[i].data(), (UINT)model.frames[i].size(), sizeof(float3), sizeof(float3), SetDataOptions::SETDATAOPTIONS_NONE);
	}


	pModel.indexBuffer = renderer->GenIndexBuffer(0, BufferUsage::BUFFERUSAGE_WRITEONLY, (UINT)(model.indexes.size() * sizeof(uint16_t)));
	renderer->SetIndexBufferData(pModel.indexBuffer, 0, (void*)model.indexes.data(), (UINT)(model.indexes.size() * sizeof(uint16_t)), SetDataOptions::SETDATAOPTIONS_DISCARD);
	pModel.pt = model.pt;
	pModel.primitiveCount = model.primitiveCount;
	pModel.indexBufferElementSize = sizeof(uint16_t) * 8;
}

void ModelsManager::ReleaseModel(size_t id) {
	auto& pModel = models[id];
	if (pModel.vertexBuffer != nullptr) {
		VertexBufferBinding* vB = pModel.vertexBuffer;
		renderer->AddDisposeIndexBuffer(pModel.indexBuffer);
		for (size_t i = 0; i < vB->buffersCount; i++) {
			renderer->AddDisposeVertexBuffer(vB->vertexBuffers[i]);
		}
		delete[] vB->vertexBuffers;
		delete vB->vertexOffset;
		delete vB->vertexStride;
		delete vB;
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
