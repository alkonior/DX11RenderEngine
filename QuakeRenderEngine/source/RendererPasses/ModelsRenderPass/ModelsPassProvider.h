#pragma once
#include <PipelineState.h>

class ModelsPassProvider : public Renderer::IStateProvider {
public:
    void PatchPipelineState(Renderer::PipelineState* refToPS, size_t definesFlags) override;
    Renderer::InputLayoutDescription GetInputLayoutDescription(size_t definesFlags) override;
    Renderer::PipelineFactoryDescription GetFactoryDescription() override;
    const char* GetShaderName() override;
    ~ModelsPassProvider() override;

};
