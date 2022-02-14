#include "UPRenderer.h"


using namespace Renderer;

UPRenderer::UPRendererProvider::UPRendererProvider(int32_t width, int32_t height) :
	width(width), height(height) {}

void UPRenderer::UPRendererProvider::PatchPipelineState(Renderer::PipelineState* refToPS, size_t definesFlags) {

	{
		refToPS->bs.enabled = false;
		refToPS->bs.colorBlendFunction = BLENDFUNCTION_ADD;;
		refToPS->bs.alphaBlendFunction = BLENDFUNCTION_ADD;
		refToPS->bs.colorSourceBlend = Blend::BLEND_ONE;
		refToPS->bs.colorDestinationBlend = Blend::BLEND_ZERO;
		refToPS->bs.alphaSourceBlend = Blend::BLEND_ONE;
		refToPS->bs.alphaDestinationBlend = Blend::BLEND_ZERO;

		refToPS->bs.colorWriteEnable = ColorWriteChannels::COLORWRITECHANNELS_ALL;
		refToPS->bs.colorWriteEnable1 = ColorWriteChannels::COLORWRITECHANNELS_ALL;
		refToPS->bs.colorWriteEnable2 = ColorWriteChannels::COLORWRITECHANNELS_ALL;
		refToPS->bs.colorWriteEnable3 = ColorWriteChannels::COLORWRITECHANNELS_ALL;

		refToPS->bs.multiSampleMask = -1;

		refToPS->bs.blendFactor = Renderer::Color{ 255,255,255,255 };

		refToPS->bf = Renderer::Color{ 255,255,255,255 };
	}
	if (definesFlags & (UPALPHA | UPWATER)) {
		refToPS->bs.enabled = true;
		refToPS->bs.colorBlendFunction = BLENDFUNCTION_ADD;;
		refToPS->bs.alphaBlendFunction = BLENDFUNCTION_ADD;
		refToPS->bs.colorSourceBlend = Blend::BLEND_SOURCEALPHA;
		refToPS->bs.colorDestinationBlend = Blend::BLEND_INVERSESOURCEALPHA;
		refToPS->bs.alphaSourceBlend = Blend::BLEND_ONE;
		refToPS->bs.alphaDestinationBlend = Blend::BLEND_ZERO;
		refToPS->bs.colorWriteEnable = ColorWriteChannels::COLORWRITECHANNELS_ALL ^ ColorWriteChannels::COLORWRITECHANNELS_ALPHA;
		refToPS->bs.colorWriteEnable1 = ColorWriteChannels::COLORWRITECHANNELS_ALL;
		refToPS->bs.colorWriteEnable2 = ColorWriteChannels::COLORWRITECHANNELS_ALL;
		refToPS->bs.colorWriteEnable3 = ColorWriteChannels::COLORWRITECHANNELS_ALL;


		refToPS->bs.multiSampleMask = -1;

		refToPS->bs.blendFactor = Renderer::Color{ 255,255,255,255 };

		refToPS->bf = Renderer::Color{ 255,255,255,255 };
	}




	refToPS->dss.depthBufferEnable = true;

	if (definesFlags & (UPALPHA | UPWATER))
		refToPS->dss.depthBufferEnable = true;
	refToPS->dss.depthBufferFunction = CompareFunction::COMPAREFUNCTION_LESSEQUAL;
	refToPS->dss.stencilEnable = false;


	refToPS->rs.cullMode = CullMode::CULLMODE_NONE;
	//refToPS->rs.cullMode = CullMode::CULLMODE_CULLCOUNTERCLOCKWISEFACE;
	refToPS->rs.depthBias = 0.0f;
	refToPS->rs.fillMode = FillMode::FILLMODE_SOLID;
	refToPS->rs.multiSampleAntiAlias = 0;
	refToPS->rs.scissorTestEnable = 0;
	refToPS->rs.slopeScaleDepthBias = 0.0f;


	refToPS->vp.x = 0;
	refToPS->vp.y = 0;
	refToPS->vp.w = width;
	refToPS->vp.h = height;
	refToPS->vp.minDepth = 0.0f;
	refToPS->vp.maxDepth = 1.0f;
}


const D3D11_INPUT_ELEMENT_DESC  DefaultInputElements[] =
{
	{ "Position",         0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD",         0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "LIGHTTEXCOORD",    0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

Renderer::InputLayoutDescription UPRenderer::UPRendererProvider::GetInputLayoutDescription(size_t definesFlags) {
	return InputLayoutDescription{ (void*)DefaultInputElements, std::size(DefaultInputElements) };
}

UPRenderer::UPRendererProvider::~UPRendererProvider() {}
