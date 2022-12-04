#pragma once
#include "PipelineState.h"

enum PostProcessRendererDefines : std::uint32_t {
    PPZERO = 0,
    PPRED = 1,
    PPSimple = 2,
    PPCopyColor = 4,
    PPWithLum = 8,
    PPWithLumEx = 16,

};

class PostProcessRenderPassProvider : public Renderer::IStateProvider {
public:
    void PatchPipelineState(Renderer::Pipeline* refToPS, uint32_t definesFlags) override;
    Renderer::InputLayoutDescription GetInputLayoutDescription(uint32_t definesFlags) override;
    Renderer::PipelineFactoryDescription GetFactoryDescription() override;
    const char* GetShaderName() override;

};
