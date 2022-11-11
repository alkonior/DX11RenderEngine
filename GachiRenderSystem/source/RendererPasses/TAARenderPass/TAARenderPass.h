#pragma once
#include "TAARenderPassConstBuffer.h"
#include "CoreRenderSystem/RenderPasses/QuadRenderPass.h"
#include "RendererPasses/GachiBasePass.h"


class TAARenderPass : public GachiBasePass {

    QuadRenderPass<0> QuadHelper;


public:
    TAARenderPass(BaseRenderSystem& System)
        : GachiBasePass({"TAAShader.hlsl", System}), QuadHelper(System.pRenderer) {}
    void Init(const char* dirr) override;
    void SetupSettings(TAASettings Settings);
    void UpdateHaltonSequence();
    void PreRender() override;

    void Resize() override;
    void Render();

    void RenderImGUI();
    void PostRender() override;
    void SetupSettings(const RenderSettings& Settings) override;

    size_t HaltonIndex;
    std::vector<float2> HaltonSequence;

private:
    TAAConstBuffer localBuffer;
    Renderer::Texture* TAAHistory;

    TAASettings Settings;
    Renderer::ConstBuffer* constBuffer;
};
