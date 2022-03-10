#pragma once
#include "IRenderer/PipelineState/PipelineFactory.h"


enum ModelDefines {
    MZERO = 0,
    MRED = 1,
    MLERP = 2,
    MBAD_UV = 4,
    MNONORMAL = 8,
    MCOLORED = 16,
    MLIGHTED = 32
};

const Renderer::ShaderDefines ModelRendererDefines[] = {
    {"RED", "1"},
    {"LERP", "1"},
    {"BAD_UV", "1"},
    {"MNONORMAL", "1"},
    {"COLORED", "1"},
    {"LIGHTED", "1"},
};



class ModelRendererFactory: public Renderer::PipelineFactory {
public:
    ModelRendererFactory(Renderer::IRenderer* renderer, Renderer::IStateProvider* provider, void* shaderData, size_t dataSize):
    PipelineFactory(renderer, provider, (const Renderer::ShaderDefines*)ModelRendererDefines, std::size(ModelRendererDefines), shaderData, dataSize,
#ifdef DEBUG 
        D3DCOMPILE_DEBUG
#else
0
#endif
) {
    };
};

