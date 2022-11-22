#pragma once
#include <unordered_map>
#include "IRenderer.h"
#include "CoreRenderSystem/ResourceManagers/IModelsManager.h"
#include "Utils/ModelData.h"


class ModelsManager : public IModelsManager<ModelData, FramedModelData> {

public:

	ModelsManager(Renderer::IRenderer* renderDevice) : IModelsManager<ModelData, FramedModelData>(renderDevice) {};

	void ReleaseModel(size_t id) override;
	SavedModel GetModel(size_t id) override;
	
	void RegisterModel(size_t id, const FramedModelData& model) override;
	void RegisterModel(size_t id, const ModelData& model) override;
	

	virtual ~ModelsManager() override;
	bool WasIdUsed(size_t id) override {return true;};
private:
	std::unordered_map<size_t, SavedModel> models = {};
};

