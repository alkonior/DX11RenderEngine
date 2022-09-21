#pragma once
#include "IRenderer.h"
#include "Utils/ModelData.h"


class ModelsManager {

public:

	struct ModelCache {
		Renderer::VertexBufferBinding vertexBuffer;
		Renderer::Buffer* indexBuffer = nullptr;
		size_t primitiveCount;
		size_t indexBufferElementSize;
		Renderer::PrimitiveType pt;

	};

	ModelsManager();

	void RegisterModel(const ModelData& model, size_t id);
	void RegisterFramedModel(const FramedModelData& model, size_t id);
	void ReleaseModel(size_t id);
	ModelCache GetModel(size_t id);
	//void RegImgFromFile(GraphicsBase gfx, size_t width, size_t height, LPCSTR file, LPCSTR name);


	~ModelsManager();
private:
	std::map<size_t, ModelCache> models;
};

