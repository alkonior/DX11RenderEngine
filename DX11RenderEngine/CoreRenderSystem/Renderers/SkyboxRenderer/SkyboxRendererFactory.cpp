#include "SkyboxRendererFactory.h"


using namespace Renderer;




SkyboxRendererFactory::SkyboxRendererFactory(IRenderer* renderer, Renderer::IStateProvider* provider, void* shaderData, size_t dataSize)
	:PipelineFactory(renderer, provider, (const Renderer::ShaderDefines*)SkyboxRendererDefines, std::size(SkyboxRendererDefines), shaderData, dataSize, 
#ifdef DEBUG 
		D3DCOMPILE_DEBUG
#else
0
#endif
	)
{

}
