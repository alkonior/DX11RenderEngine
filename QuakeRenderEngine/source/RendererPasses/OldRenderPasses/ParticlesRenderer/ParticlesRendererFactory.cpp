#include "pch.h"
#include "ParticlesRendererFactory.h"

ParticlesRendererFactory::ParticlesRendererFactory(Renderer::IStateProvider* provider, void* shaderData, size_t dataSize) :
	Renderer::PipelineFactory(provider, (const ShaderDefines*)ParticlesRendererDefines, std::size(ParticlesRendererDefines), shaderData, dataSize,
		Renderer::UsedShaders(Renderer::UseGeometryShader /* | Renderer::UseComputeShader */), 
#ifdef _DEBUG 
(1 << 0)
#else
0
#endif
) {}
