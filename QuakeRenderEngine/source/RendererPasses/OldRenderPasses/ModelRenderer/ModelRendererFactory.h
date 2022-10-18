#pragma once
#include "CoreRenderSystem/PipelineFactory.h"





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

