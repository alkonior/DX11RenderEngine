#pragma once
#include "IRenderer.h"
#include <utility>

template <class ...T>
class IModelsManager;

template <>
class IModelsManager<> {

public:
	struct SavedModel {
		Renderer::VertexBufferBinding vertexBuffer;
		Renderer::Buffer* indexBuffer = nullptr;
		size_t primitiveCount;
		size_t indexBufferElementSize;
		Renderer::PrimitiveType pt;

	};

	
	virtual void ReleaseModel(size_t id) = 0;
	virtual SavedModel GetModel(size_t id) = 0;
	
	virtual ~IModelsManager() = default;
};

template <class ModelType, class... T>
class IModelsManager<ModelType, T...> : public IModelsManager<T...>{
public:
	
	virtual void RegisterModel(size_t id, const ModelType& model) = 0;
	
};