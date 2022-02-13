#include "UIRendererFactory.h"


using namespace Renderer;

const Renderer::ShaderDefines UIRendererDefines[ ] = {
	{"RED", "1"},
	{"COLORED", "1"}
};


UIRendererFactory::UIRendererFactory(IRenderer* renderer, Renderer::IStateProvider* provider, void* shaderData, size_t dataSize)
	:PipelineFactory(renderer, provider, (const Renderer::ShaderDefines*)UIRendererDefines, std::size(UIRendererDefines), shaderData, dataSize, D3DCOMPILE_DEBUG)
{

}