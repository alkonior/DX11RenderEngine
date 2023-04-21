#include "UPRenderer.h"


using namespace Renderer;

UPRenderer::UPRendererProvider::UPRendererProvider(int32_t width, int32_t height) :
	width(width), height(height) {}

void UPRenderer::UPRendererProvider::PatchPipelineState(Renderer::PipelineState* refToPS, size_t definesFlags) {

	
	refToPS->bs = &BlendStates::NoAlpha;

	refToPS->dss = &DepthStencilStates::DSS;
	refToPS->rs = &RasterizerStates::ClockWise;
	
	if (definesFlags & (UPALPHA)) {
		refToPS->dss = &DepthStencilStates::NoWrite;
	}
}


const D3D11_INPUT_ELEMENT_DESC  DefaultInputElements[] =
{
	{ "Position",         0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL",           0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD",         0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "LIGHTTEXCOORD",    0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "DLIGHTTEXCOORD",    0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

Renderer::InputLayoutDescription UPRenderer::UPRendererProvider::GetInputLayoutDescription(size_t definesFlags) {
	return InputLayoutDescription{ (void*)DefaultInputElements, std::size(DefaultInputElements) };
}

const char* UPRenderer::UPRendererProvider::GetShaderName()
{
	return "BSP shader";
}

