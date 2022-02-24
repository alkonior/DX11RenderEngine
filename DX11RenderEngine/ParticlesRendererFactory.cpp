#include "ParticlesRendererFactory.h"

ParticlesRendererFactory::ParticlesRendererFactory(Renderer::IRenderer* renderer, Renderer::IStateProvider* provider, void* shaderData, size_t dataSize) :
	Renderer::PipelineFactory(renderer, provider, (const ShaderDefines*)ParticlesRendererDefines, std::size(ParticlesRendererDefines), shaderData, dataSize,
		Renderer::UsedShaders(Renderer::UseGeometryShader /* | Renderer::UseComputeShader */), D3DCOMPILE_DEBUG) {}