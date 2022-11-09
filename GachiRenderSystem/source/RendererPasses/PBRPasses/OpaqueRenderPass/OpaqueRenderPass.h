#pragma once
#include "CoreRenderSystem/RenderPasses/BaseRenderPass.h"
#include "OpaquePassProvider.h"
#include "ResourceManagers/ModelsManager.h"
#include "ResourceManagers/TexturesManager.h"
#include "OpaquePassConstBuffer.h"
#include "RendererPasses/GachiBasePass.h"
#include "Utils/OpaqueModelDrawData.h"

class OpaqueRenderPass : public GachiBasePass{

    struct DrawCall {
        DrawCall(ModelsManager::SavedModel model, TexturesManager::TextureCache texture, const OpaqueModelDrawData&);

        ModelsManager::SavedModel model;
        TexturesManager::TextureCache diffuse;
        TexturesManager::TextureCache normal;
        TexturesManager::TextureCache texture;
        TexturesManager::TextureCache texture;
        OpaqueModelDrawData data;
    };	
	
    PipelineFactoryFlags ParseFlags(const OpaqueFlags& flags);
    
public:
    explicit OpaqueRenderPass(BaseRenderSystem& in);
    
    void Draw(const OpaqueModelDrawData& drawData);
    
    void Init(const char* dirr) override;
    void PreRender() override;


    void Resize() override;
    void Render();
    
    void PostRender() override;
    ~OpaqueRenderPass() override;
    void SetupSettings(const RenderSettings& Settings) override;

private:
    OpaqueCosntBuffer dataBuffer;

    Renderer::ConstBuffer* pDataCB;

    std::vector<DrawCall> drawCalls;
	Renderer::Viewport vp;
};
