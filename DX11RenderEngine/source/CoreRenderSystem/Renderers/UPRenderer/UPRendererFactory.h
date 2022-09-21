#pragma once
#include "CoreRenderSystem/PipelineFactory.h"



using Renderer::ShaderDefines;

const ShaderDefines UPRendererDefines[] = {
    ShaderDefines("RED"),
    ShaderDefines("ALPHA"),
    ShaderDefines("LIGHTMAPPED")
};


class UPRendererFactory : public Renderer::PipelineFactory  {
public:
    UPRendererFactory(Renderer::IStateProvider* provider, void* shaderData, size_t dataSize);
};

