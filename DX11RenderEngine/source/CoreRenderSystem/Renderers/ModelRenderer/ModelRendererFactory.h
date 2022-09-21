#pragma once
#include "CoreRenderSystem/PipelineFactory.h"



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
    ModelRendererFactory(Renderer::IStateProvider* provider, void* shaderData, size_t dataSize):
    PipelineFactory(provider, (const Renderer::ShaderDefines*)ModelRendererDefines, std::size(ModelRendererDefines), shaderData, dataSize,
#ifdef _DEBUG 
    (1 << 0)
#else
0
#endif
) {
    };
};

