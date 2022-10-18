#include "pch.h"
#include "UPRendererFactory.h"

using namespace Renderer;


UPRendererFactory::UPRendererFactory(IStateProvider* provider, void* shaderData, size_t dataSize):
	Renderer::PipelineFactory(provider, (const ShaderDefines*)UPRendererDefines, std::size(UPRendererDefines), shaderData, dataSize,
#ifdef _DEBUG 
(1 << 0)
#else
0
#endif
) {}
