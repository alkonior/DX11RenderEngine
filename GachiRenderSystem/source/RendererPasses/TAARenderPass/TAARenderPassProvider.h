#pragma once
#include "PipelineState.h"

enum  TAARendererDefines : std::uint32_t {
    TAAZERO = 0,
    TAARED = 1,
    TAACOPY = 2,
    TAAUSEYUV = 4,

};

class TAARenderPassProvider : public Renderer::IStateProvider {
public:
    void PatchPipelineState(Renderer::Pipeline* refToPS, uint32_t definesFlags) override;
    Renderer::InputLayoutDescription GetInputLayoutDescription(uint32_t definesFlags) override;
    Renderer::PipelineFactoryDescription GetFactoryDescription() override;
    const char* GetShaderName() override;

};
