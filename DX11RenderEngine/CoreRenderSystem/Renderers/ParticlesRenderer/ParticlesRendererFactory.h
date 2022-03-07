#pragma once
#include "IRenderer/PipelineState/PipelineFactory.h"


enum particlesDefines : uint64_t {
    PARZERO = 0,
    PARRED = 1,
};

using Renderer::ShaderDefines;

const ShaderDefines ParticlesRendererDefines[] = {
    ShaderDefines("RED"),
};


class ParticlesRendererFactory : public Renderer::PipelineFactory {
public:
    ParticlesRendererFactory(Renderer::IRenderer* renderer, Renderer::IStateProvider* provider, void* shaderData, size_t dataSize);
};
