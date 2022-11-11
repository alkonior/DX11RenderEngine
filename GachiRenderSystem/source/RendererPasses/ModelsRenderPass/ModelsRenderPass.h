#pragma once
#include "CoreRenderSystem/RenderPasses/BaseRenderPass.h"
#include "ModelsPassProvider.h"
#include "ResourceManagers/ModelsManager.h"
#include "ResourceManagers/TexturesManager.h"
#include "ModelsPassConstBuffer.h"
#include "RendererPasses/GachiBasePass.h"
#include "Utils/DrawData.h"

class ModelsRenderPass : public GachiBasePass{

    struct DrawCall {
        DrawCall(ModelsManager::SavedModel model, TexturesManager::TextureCache texture, const ModelDrawData&);

        ModelsManager::SavedModel model;
        TexturesManager::TextureCache texture;
        ModelDrawData data;
    };	
	
    PipelineFactoryFlags ParseFlags(const ModelsFlags& flags);
    
public:
    explicit ModelsRenderPass(BaseRenderSystem& in);
    
    void Draw(const ModelDrawData& drawData);
    
    void Init(const char* dirr) override;
    void PreRender() override;


    void Resize() override;
    void Render();
    
    void PostRender() override;
    ~ModelsRenderPass() override;
    void SetupSettings(const RenderSettings& Settings) override;

private:
    ModelsCosntBuffer dataBuffer;

    Renderer::ConstBuffer* pDataCB;

    std::vector<DrawCall> drawCalls;
	Renderer::Viewport vp;
};
