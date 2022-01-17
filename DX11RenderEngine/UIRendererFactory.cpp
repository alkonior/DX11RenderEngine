#include "UIRendererFactory.h"


using namespace Renderer;

Renderer::ShaderDefines UIRendererDefines[ ] = {
	{"COLORED", "1"},
	{"FADE", "1"},
	{"TeXTURED", "1"}
};

D3D11_INPUT_ELEMENT_DESC UIRendererInputLayout[ ] =
{
		{"Position",  0, DXGI_FORMAT_R32G32_FLOAT,  0,                            0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
};

UIRendererFactory::UIRendererFactory(IRenderer* renderer, Renderer::IStateProvider* provider, void* shaderData, size_t dataSize)
	:PipelineFactory(renderer, provider, UIRendererDefines, sizeof(UIRendererDefines), shaderData, dataSize, (void*)UIRendererInputLayout, std::size(UIRendererInputLayout))
{

}
