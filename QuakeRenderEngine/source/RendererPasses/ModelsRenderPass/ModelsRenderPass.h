#pragma once
#include "CoreRenderSystem/RenderPasses/BaseRenderPass.h"
#include "ModelsPassProvider.h"
#include "ResourceManagers/ModelsManager.h"
#include "ResourceManagers/TexturesManager.h"
#include "ModelsPassConstBuffer.h"
#include "Utils/DrawData.h"

class ModelsRenderPass : public BaseRenderPass{

    struct DrawCall {
        DrawCall(ModelsManager::SavedModel model, TexturesManager::TextureCache texture, Transform position, size_t flags);

        ModelsManager::SavedModel model; TexturesManager::TextureCache texture; Transform position; size_t flags;
    };	
	
    struct DrawLerpCall {
        DrawLerpCall(ModelsManager::SavedModel model, TexturesManager::TextureCache texture, const LerpModelDrawData&);

        ModelsManager::SavedModel model; TexturesManager::TextureCache texture; 
        LerpModelDrawData data;
    };
    
public:
    explicit ModelsRenderPass(BaseRenderSystem& in);
    
    void Draw(ModelsManager::SavedModel model, TexturesManager::TextureCache texture, Transform position, size_t flags);
    void DrawLerp(ModelsManager::SavedModel model, TexturesManager::TextureCache texture, const LerpModelDrawData&);

    
    void Init(const char* dirr) override;
    void PreRender() override;

    
    void Render();
    
    void PostRender() override;
    ~ModelsRenderPass() override;

private:
    ModelsCosntBuffer dataBuffer;

    Renderer::ConstBuffer* pDataCB;

    std::vector<DrawCall> drawCalls;
    std::vector<DrawLerpCall> drawLerpCalls;
	Renderer::Viewport vp;
};
