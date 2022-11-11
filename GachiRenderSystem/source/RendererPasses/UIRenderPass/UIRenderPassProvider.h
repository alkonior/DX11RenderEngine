#pragma once
#include <PipelineState.h>

class UIRenderPassProvider : public Renderer::IStateProvider {
public:
    void PatchPipelineState(Renderer::Pipeline* refToPS, uint32_t definesFlags) override;
    Renderer::InputLayoutDescription GetInputLayoutDescription(uint32_t definesFlags) override;
    Renderer::PipelineFactoryDescription GetFactoryDescription() override;
    const char* GetShaderName() override;

};
