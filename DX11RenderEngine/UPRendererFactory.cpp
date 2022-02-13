#include "UPRendererFactory.h"

using namespace Renderer;

const ShaderDefines UPRendererFactory::UPRendererDefines[] = {
    {"RED", "1"},
    {"POSITIONED", "1"},
};

UPRendererFactory::UPRendererFactory(IRenderer* renderer, IStateProvider* provider, void* shaderData, size_t dataSize):
	Renderer::PipelineFactory(renderer, provider, (const ShaderDefines*)UPRendererDefines, std::size(UPRendererDefines), shaderData, dataSize, D3DCOMPILE_DEBUG) {}