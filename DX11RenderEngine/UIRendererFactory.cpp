#include "UIRendererFactory.h"


using namespace Renderer;

Renderer::ShaderDefines UIRendererDefines[ ] = {
	{"RED", "1"},
	{"FADE", "1"},
	{"TEXTURED", "1"}
};


const D3D11_INPUT_ELEMENT_DESC Vertex2D::InputElements[ ] =
{
		{"Position",  0, DXGI_FORMAT_R32G32_FLOAT,  0,                            0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
};
UIRendererFactory::UIRendererFactory(IRenderer* renderer, Renderer::IStateProvider* provider, void* shaderData, size_t dataSize)
	:PipelineFactory(renderer, provider, UIRendererDefines, std::size(UIRendererDefines), shaderData, dataSize, (void*)Vertex2D::InputElements, std::size(Vertex2D::InputElements), D3DCOMPILE_DEBUG)
{

}
