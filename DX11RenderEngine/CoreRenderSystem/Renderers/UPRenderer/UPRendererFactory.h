#pragma once
#include "IRenderer/PipelineState/PipelineFactory.h"

enum UPDefines  : uint64_t {
    UPZERO = 0,
    UPRED = 1,
    UPALPHA = 2,
    UPLIGHTMAPPED = 4,
    DYNAMIC = 8
};

using Renderer::ShaderDefines;

const ShaderDefines UPRendererDefines[] = {
    ShaderDefines("RED"),
    ShaderDefines("ALPHA"),
    ShaderDefines("LIGHTMAPPED"),
    ShaderDefines("DYNAMIC"),
};


class UPRendererFactory : public Renderer::PipelineFactory  {
public:
    UPRendererFactory(Renderer::IStateProvider* provider, void* shaderData, size_t dataSize);
};

