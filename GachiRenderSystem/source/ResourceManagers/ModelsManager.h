#pragma once
#include <unordered_map>
#include "IRenderer.h"
#include "CoreRenderSystem/ResourceManagers/IModelsManager.h"
#include "Utils/ModelData.h"


class ModelsManager : public IModelsManager<ModelData> {

public:

	ModelsManager(Renderer::IRenderer* renderDevice) : IModelsManager<ModelData>(renderDevice) {};

	void ReleaseModel(size_t id) override;
	SavedModel GetModel(size_t id) override;
	
	void RegisterModel(size_t id, const ModelData& model) override;
	

	virtual ~ModelsManager() override;
private:
	std::unordered_map<size_t, SavedModel> models = {};
};

