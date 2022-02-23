#include "UPRendererFactory.h"

using namespace Renderer;


UPRendererFactory::UPRendererFactory(IRenderer* renderer, IStateProvider* provider, void* shaderData, size_t dataSize):
	Renderer::PipelineFactory(renderer, provider, (const ShaderDefines*)UPRendererDefines, std::size(UPRendererDefines), shaderData, dataSize, D3DCOMPILE_DEBUG) {}
