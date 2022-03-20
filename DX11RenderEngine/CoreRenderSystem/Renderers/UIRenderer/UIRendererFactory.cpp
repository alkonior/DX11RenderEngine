#include "UIRendererFactory.h"


using namespace Renderer;




UIRendererFactory::UIRendererFactory(Renderer::IStateProvider* provider, void* shaderData, size_t dataSize)
	:PipelineFactory(provider, (const Renderer::ShaderDefines*)UIRendererDefines, std::size(UIRendererDefines), shaderData, dataSize,
#ifdef _DEBUG 
		D3DCOMPILE_DEBUG
#else
0
#endif
)
{

}
