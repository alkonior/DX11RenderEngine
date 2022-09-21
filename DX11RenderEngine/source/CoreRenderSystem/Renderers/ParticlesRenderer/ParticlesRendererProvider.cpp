#include "pch.h"

#include <d3d11.h>

#include "ParticlesRenderer.h"
using namespace Renderer;

ParticlesRenderer::ParticlesRendererProvider::ParticlesRendererProvider(int32_t width, int32_t height) :
	width(width), height(height) {}

void ParticlesRenderer::ParticlesRendererProvider::PatchPipelineState(Renderer::PipelineState* refToPS, size_t definesFlags) {

	refToPS->bs = &BlendStates::Alpha;


	refToPS->dss = &DepthStencilStates::NoWrite;
	refToPS->rs = &RasterizerStates::ClockWise;

}


const D3D11_INPUT_ELEMENT_DESC  DefaultInputElements[] =
{
	{ "Position",         0, DXGI_FORMAT_R32G32B32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "Color",            0, DXGI_FORMAT_R32G32B32A32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

Renderer::InputLayoutDescription ParticlesRenderer::ParticlesRendererProvider::GetInputLayoutDescription(size_t definesFlags) {
	return InputLayoutDescription{ (void*)DefaultInputElements, std::size(DefaultInputElements) };
}

const char* ParticlesRenderer::ParticlesRendererProvider::GetShaderName()
{
	return "Particles shader";
}

