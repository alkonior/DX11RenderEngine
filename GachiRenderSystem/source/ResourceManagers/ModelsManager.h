#pragma once
#include <unordered_map>
#include "IRenderer.h"
#include "CoreRenderSystem/ResourceManagers/IModelsManager.h"
#include "CoreRenderSystem/ResourceManagers/ITexturesManager.h"
#include "Utils/ModelMesh.h"


class ModelsManager : public IModelsManager<ModelMesh, OpaqueMesh> {

public:

	ModelsManager(Renderer::IRenderer* renderDevice) : IModelsManager<ModelMesh, OpaqueMesh>(renderDevice) {};

	void ReleaseModel(size_t id) override;
	SavedModel GetModel(size_t id) override;
	
	template<class T>
	void UniversalRegisterModel(size_t id, const MeshData<T>& model)
	{
		auto& pModel = models[id];
		if (pModel.vertexBuffer.buffersCount != 0)
			ReleaseModel(id);

		Renderer::VertexBufferBinding vertexBuffer;


		vertexBuffer.buffersCount = 1;
		vertexBuffer.vertexBuffers = new Renderer::Buffer * [1];
		vertexBuffer.vertexBuffers[0] = renderDevice->GenVertexBuffer(
			0, Renderer::BufferUsage::BUFFERUSAGE_NONE,
			static_cast<int32_t>(sizeof(T) * model.vertices.size()));
		vertexBuffer.vertexOffset = new UINT[1];
		vertexBuffer.vertexOffset[0] = 0;
		vertexBuffer.vertexStride = new UINT[1];
		vertexBuffer.vertexStride[0] = sizeof(T);
		renderDevice->SetVertexBufferData(vertexBuffer.vertexBuffers[0], 0, (void*)model.vertices.data(), (UINT)model.vertices.size(), (UINT)sizeof(T), (UINT)sizeof(T), Renderer::SetDataOptions::SETDATAOPTIONS_NONE);


		pModel.vertexBuffer = vertexBuffer;
		pModel.indexBuffer = renderDevice->GenIndexBuffer(
			0, Renderer::BufferUsage::BUFFERUSAGE_WRITEONLY,
			(UINT)(model.indexes.size() * sizeof(uint32_t)));
		renderDevice->SetIndexBufferData(
			pModel.indexBuffer, 0, (void*)model.indexes.data(),
			(UINT)(model.indexes.size() * sizeof(uint32_t)), Renderer::SetDataOptions::SETDATAOPTIONS_DISCARD);
		pModel.pt = (Renderer::PrimitiveType)model.pt;
		pModel.primitiveCount = model.primitiveCount;
		pModel.indexBufferElementSize = sizeof(uint32_t) * 8;
	}

	
	void RegisterModel(size_t id, const ModelMesh& model) override;
	void RegisterModel(size_t id, const OpaqueMesh& model) override;
	
	virtual bool WasIdUsed(size_t id) override;
	
	virtual ~ModelsManager() override;
private:
	std::unordered_map<size_t, SavedModel> models = {};
};

