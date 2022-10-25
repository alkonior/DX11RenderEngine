#pragma once
#include "TAARenderPassConstBuffer.h"
#include "CoreRenderSystem/RenderPasses/QuadRenderPass.h"

struct TAASettings {
    bool markNoHistoryPixels;
    bool allowBicubicFilter;
    bool allowDepthThreshold;
    bool allowVarianceClipping;
    bool allowNeighbourhoodSampling;
    bool allowYCoCg;
    bool allowLongestVelocityVector;
    int numSamples;
    float taaStrength;
};

class TAARenderPass : public BaseRenderPass {

    QuadRenderPass<0> QuadHelper;


public:
    TAARenderPass(BaseRenderSystem& System)
        : BaseRenderPass({"TAAShader.hlsl", System}), QuadHelper(System.pRenderer) {}
    void Init(const char* dirr) override;
    void SetupSettings(TAASettings Settings);
    void UpdateHaltonSequence();
    void PreRender() override;

    void Render();

    void RenderImGUI();
    void PostRender() override;
    
    size_t HaltonIndex;
    std::vector<float2> HaltonSequence;

private:
    TAAConstBuffer localBuffer;
    Renderer::Texture* TAAHistory;

    TAASettings Settings;
    Renderer::ConstBuffer* constBuffer;
};
