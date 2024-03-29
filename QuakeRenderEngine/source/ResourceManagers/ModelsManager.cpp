﻿#include "ModelsManager.h"

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

void ModelsManager::RegisterModel(size_t id, const FramedModelData& model)
{
	auto& pModel = models[id];
	if (pModel.vertexBuffer.buffersCount != 0)
		ReleaseModel(id);

	VertexBufferBinding vertexBuffer;


	const size_t framesCount = model.frames.size();
	vertexBuffer.buffersCount = framesCount + 1;
	vertexBuffer.vertexBuffers = new Buffer * [framesCount + 1];
	vertexBuffer.vertexOffset = new UINT[framesCount + 1];
	vertexBuffer.vertexStride = new UINT[framesCount + 1];

	vertexBuffer.vertexBuffers[0] = renderDevice->GenVertexBuffer(0, BufferUsage::BUFFERUSAGE_NONE, static_cast<int32_t>(sizeof(FramedModelCommon) * model.verticies.size()));
	vertexBuffer.vertexOffset[0] = 0;
	vertexBuffer.vertexStride[0] = sizeof(FramedModelCommon);
	renderDevice->SetVertexBufferData(vertexBuffer.vertexBuffers[0], 0, (void*)model.verticies.data(), (UINT)model.verticies.size(), (UINT)sizeof(FramedModelCommon), (UINT)sizeof(FramedModelCommon), SetDataOptions::SETDATAOPTIONS_NONE);

	for (size_t i = 0; i < framesCount; i++) {
		vertexBuffer.vertexBuffers[i + 1] = renderDevice->GenVertexBuffer(0, BufferUsage::BUFFERUSAGE_NONE, static_cast<int32_t>(sizeof(FramedModelVertex) * model.frames[i].size()));
		vertexBuffer.vertexOffset[i + 1] = 0;
		vertexBuffer.vertexStride[i + 1] = sizeof(FramedModelVertex);
		renderDevice->SetVertexBufferData(vertexBuffer.vertexBuffers[i + 1], 0, (void*)model.frames[i].data(), (UINT)model.frames[i].size(), sizeof(FramedModelVertex), sizeof(FramedModelVertex), SetDataOptions::SETDATAOPTIONS_NONE);
	}


	pModel.vertexBuffer = vertexBuffer;
	pModel.indexBuffer = renderDevice->GenIndexBuffer(0, BufferUsage::BUFFERUSAGE_WRITEONLY, (UINT)(model.indexes.size() * sizeof(uint16_t)));
	renderDevice->SetIndexBufferData(pModel.indexBuffer, 0, (void*)model.indexes.data(), (UINT)(model.indexes.size() * sizeof(uint16_t)), SetDataOptions::SETDATAOPTIONS_DISCARD);
	pModel.pt = (Renderer::PrimitiveType)model.pt;
	pModel.primitiveCount = model.primitiveCount;
	pModel.indexBufferElementSize = sizeof(uint16_t) * 8;
}

void ModelsManager::RegisterModel(size_t id, const ModelData& model)
{
	auto& pModel = models[id];
	if (pModel.vertexBuffer.buffersCount != 0)
		ReleaseModel(id);

	VertexBufferBinding vertexBuffer;


	vertexBuffer.vertexBuffers = new Buffer * [1];
	vertexBuffer.vertexBuffers[0] = renderDevice->GenVertexBuffer(0, BufferUsage::BUFFERUSAGE_NONE, static_cast<int32_t>(sizeof(ModelVertex) * model.verticies.size()));
	vertexBuffer.vertexOffset = new UINT[1];
	vertexBuffer.vertexOffset[0] = 0;
	vertexBuffer.vertexStride = new UINT[1];
	vertexBuffer.vertexStride[0] = sizeof(ModelVertex);
	renderDevice->SetVertexBufferData(vertexBuffer.vertexBuffers[0], 0, (void*)model.verticies.data(), (UINT)model.verticies.size(), (UINT)sizeof(ModelVertex), (UINT)sizeof(ModelVertex), SetDataOptions::SETDATAOPTIONS_NONE);


	pModel.vertexBuffer = vertexBuffer;
	pModel.indexBuffer = renderDevice->GenIndexBuffer(0, BufferUsage::BUFFERUSAGE_WRITEONLY, (UINT)(model.indexes.size() * sizeof(uint16_t)));
	renderDevice->SetIndexBufferData(pModel.indexBuffer, 0, (void*)model.indexes.data(), (UINT)(model.indexes.size() * sizeof(uint16_t)), SetDataOptions::SETDATAOPTIONS_DISCARD);
	pModel.pt = (Renderer::PrimitiveType)model.pt;
	pModel.primitiveCount = model.primitiveCount;
	pModel.indexBufferElementSize = sizeof(uint16_t) * 8;
}
