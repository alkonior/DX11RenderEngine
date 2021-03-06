#include "ParticlesRendererFactory.h"

ParticlesRendererFactory::ParticlesRendererFactory(Renderer::IStateProvider* provider, void* shaderData, size_t dataSize) :
	Renderer::PipelineFactory(provider, (const ShaderDefines*)ParticlesRendererDefines, std::size(ParticlesRendererDefines), shaderData, dataSize,
		Renderer::UsedShaders(Renderer::UseGeometryShader /* | Renderer::UseComputeShader */), 
#ifdef _DEBUG 
D3DCOMPILE_DEBUG
#else
0
#endif
) {}
