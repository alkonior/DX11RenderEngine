#pragma once
#include "PipelineFactory.h"
#include "ParticlesUtils.h"
enum UPDefines : uint64_t {
    UPZERO = 0,
    UPRED = 1,
};

using Renderer::ShaderDefines;

const ShaderDefines ParticlesRendererDefines[] = {
    ShaderDefines("RED"),
};


class ParticlesRendererFactory : public Renderer::PipelineFactory {
public:
    ParticlesRendererFactory(Renderer::IRenderer* renderer, Renderer::IStateProvider* provider, void* shaderData, size_t dataSize);
};
