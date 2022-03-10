#include "ParticlesRenderer.h"
using namespace Renderer;

ParticlesRenderer::ParticlesRendererProvider::ParticlesRendererProvider(int32_t width, int32_t height) :
	width(width), height(height) {}

void ParticlesRenderer::ParticlesRendererProvider::PatchPipelineState(Renderer::PipelineState* refToPS, size_t definesFlags) {

	refToPS->bs = &BlendStates::Alpha;




	refToPS->dss.depthBufferEnable = true;
	refToPS->dss.depthBufferFunction = CompareFunction::COMPAREFUNCTION_LESSEQUAL;
	refToPS->dss.stencilEnable = false;

	refToPS->dss.depthBufferWriteEnable = true;


	refToPS->rs.cullMode = CullMode::CULLMODE_NONE;
	refToPS->rs.depthBias = 0.0f;
	refToPS->rs.fillMode = FillMode::FILLMODE_SOLID;
	refToPS->rs.multiSampleAntiAlias = 0;
	refToPS->rs.scissorTestEnable = 0;
	refToPS->rs.slopeScaleDepthBias = 0.0f;

}


const D3D11_INPUT_ELEMENT_DESC  DefaultInputElements[] =
{
	{ "Position",         0, DXGI_FORMAT_R32G32B32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "Color",            0, DXGI_FORMAT_R32G32B32A32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

Renderer::InputLayoutDescription ParticlesRenderer::ParticlesRendererProvider::GetInputLayoutDescription(size_t definesFlags) {
	return InputLayoutDescription{ (void*)DefaultInputElements, std::size(DefaultInputElements) };
}

ParticlesRenderer::ParticlesRendererProvider::~ParticlesRendererProvider() {}
