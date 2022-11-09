#pragma once
#include "IRenderer.h"
#include <utility>

template <class ...T>
class IModelsManager;

template <>
class IModelsManager<> {
protected:
	Renderer::IRenderer* renderDevice = nullptr;
public:
	IModelsManager(Renderer::IRenderer* renderDevice) : renderDevice(renderDevice) {};

	
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
	IModelsManager<ModelType, T...>(Renderer::IRenderer* renderDevice) : IModelsManager<T...>(renderDevice) {};
	virtual void RegisterModel(size_t id, const ModelType& model) = 0;
	
};