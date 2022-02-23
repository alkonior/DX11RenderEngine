#pragma once
#include "PipelineFactory.h"
#include "UPRendererUtils.h"
enum UPDefines  : uint64_t {
    UPZERO = 0,
    UPRED = 1,
    UPALPHA = 2,
    UPLIGHTMAPPED = 4
};

using Renderer::ShaderDefines;

const ShaderDefines UPRendererDefines[] = {
    ShaderDefines("RED"),
    ShaderDefines("ALPHA"),
    ShaderDefines("LIGHTMAPPED")
};


class UPRendererFactory : public Renderer::PipelineFactory  {
public:
    UPRendererFactory(Renderer::IRenderer* renderer, Renderer::IStateProvider* provider, void* shaderData, size_t dataSize);
};

