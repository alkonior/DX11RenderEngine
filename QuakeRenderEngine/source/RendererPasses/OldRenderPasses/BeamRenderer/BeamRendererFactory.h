#pragma once
#include "CoreRenderSystem//PipelineFactory.h"


enum beamDefines : uint64_t {
    BEAMZERO = 0,
    BEAMRED = 1,
};

using Renderer::ShaderDefines;

const ShaderDefines BeamRendererDefines[] = {
    ShaderDefines("RED"),
};


class BeamRendererFactory : public Renderer::PipelineFactory {
public:
    BeamRendererFactory(Renderer::IStateProvider* provider, void* shaderData, size_t dataSize) :
    Renderer::PipelineFactory(provider, (const ShaderDefines*)BeamRendererDefines, std::size(BeamRendererDefines), shaderData, dataSize,
        Renderer::UsedShaders(Renderer::UseGeometryShader /* | Renderer::UseComputeShader */), 
#ifdef _DEBUG 
1
#else
0
#endif
) {};

};
