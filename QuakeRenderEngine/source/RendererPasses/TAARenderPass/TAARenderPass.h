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
    TAARenderPass(BaseRenderSystem& System);
    void Init(const char* dirr) override;
    void UpdateHaltonSequence();
    void PreRender() override;

    void Resize() override;
    void Render();

    void RenderImGUI();
    void PostRender() override;

    size_t HaltonIndex;
    std::vector<float2> HaltonSequence;
    TAASettings Settings;
private:
    TAAConstBuffer localBuffer;
    Renderer::Texture* TAAHistory;

    Renderer::ConstBuffer* constBuffer;
};
