#pragma once
#include <PipelineState.h>

class ModelsPassProvider : public Renderer::IStateProvider {
public:
    void PatchPipelineState(Renderer::Pipeline* refToPS, uint32_t pipelineFlags) override;
    Renderer::InputLayoutDescription GetInputLayoutDescription(uint32_t pipelineFlags) override;
    Renderer::PipelineFactoryDescription GetFactoryDescription() override;
    const char* GetShaderName() override;
    ~ModelsPassProvider() override;

};
