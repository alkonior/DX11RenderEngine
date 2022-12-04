#pragma once
#include "PostProcessRenderPassConstBuffer.h"
#include "CoreRenderSystem/RenderPasses/QuadRenderPass.h"
#include "RendererPasses/GachiBasePass.h"


class PostProcessRenderPass : public GachiBasePass {

    QuadRenderPass<0> QuadHelper;


public:
    
    PostProcessRenderPass(BaseRenderSystem& System);
    void Init(const char* dirr) override;
    void SetupSettings(PostProcessSettings Settings);
    void PreRender() override;

    void Resize() override;
    void Render();

    void RenderImGUI();
    void PostRender() override;
    void SetupSettings(const RenderSettings& Settings) override;

    size_t HaltonIndex;
    std::vector<float2> HaltonSequence;

private:
    PostProcessConstantData localBuffer;
    Renderer::Texture* PostProcessHistory;

    PostProcessSettings Settings;
    Renderer::ConstBuffer* constBuffer;
};
