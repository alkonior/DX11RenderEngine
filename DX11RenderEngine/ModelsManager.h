#pragma once

#include "GraphicsBase.h"
#include "ModelData.h"


class ModelsManager {

	Renderer::IRenderer* renderer;
public:

	struct ModelCache {
		Renderer::VertexBufferBinding* vertexBuffer = nullptr;
		Renderer::Buffer* indexBuffer = nullptr;
		size_t primitiveCount;
		size_t indexBufferElementSize;
		Renderer::PrimitiveType pt;

	};

public:
	ModelsManager(Renderer::IRenderer* renderer);

	void RegisterModel(const ModelData& model, size_t id);
	void RegisterFramedModel(const FramedModelData& model, size_t id);
	void ReleaseModel(size_t id);
	ModelCache GetModel(size_t id);
	//void RegImgFromFile(GraphicsBase gfx, size_t width, size_t height, LPCSTR file, LPCSTR name);


	~ModelsManager();
private:
	std::map<size_t, ModelCache> models;
};

