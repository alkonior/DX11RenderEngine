#include "pch.h"

#include <d3d11.h>

#include "ParticlesRenderPass.h"
#include "ResourceManagers/States/BlendStates.h"
#include "ResourceManagers/States/DSStates.h"
#include "ResourceManagers/States/RasterizerStates.h"
using namespace Renderer;

ParticlesRenderPass::ParticlesRendererProvider::ParticlesRendererProvider(int32_t width, int32_t height) :
	width(width), height(height) {}

void ParticlesRenderPass::ParticlesRendererProvider::PatchPipelineState(Renderer::Pipeline* refToPS, uint32_t definesFlags) {

	refToPS->bs = &BlendStates::Alpha;


	refToPS->dss = &DepthStencilStates::NoWrite;
	refToPS->rs = &RasterizerStates::ClockWise;

}


const D3D11_INPUT_ELEMENT_DESC  DefaultInputElements[] =
{
	{ "Position",         0, DXGI_FORMAT_R32G32B32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "Color",            0, DXGI_FORMAT_R32G32B32A32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

Renderer::InputLayoutDescription ParticlesRenderPass::ParticlesRendererProvider::GetInputLayoutDescription(size_t definesFlags) {
	return InputLayoutDescription{ (void*)DefaultInputElements, std::size(DefaultInputElements) };
}
Renderer::PipelineFactoryDescription ParticlesRenderPass::ParticlesRendererProvider::GetFactoryDescription()
{

	return {
		ParticlesRendererDefines,
		sizeof(ParticlesRendererDefines),
		1
	};
	
}

const char* ParticlesRenderPass::ParticlesRendererProvider::GetShaderName()
{
	return "Particles shader";
}

