#include "UPRendererFactory.h"

using namespace Renderer;


UPRendererFactory::UPRendererFactory(IStateProvider* provider, void* shaderData, size_t dataSize):
	Renderer::PipelineFactory(provider, (const ShaderDefines*)UPRendererDefines, std::size(UPRendererDefines), shaderData, dataSize,
#ifdef _DEBUG 
D3DCOMPILE_DEBUG
#else
0
#endif
) {}
