#include "pch.h"
#include "TAARenderer.h"

#include <d3d11.h>

#include "imgui/imgui.h"
#include "halton.h"

using namespace Renderer;

TAARenderer::TAARendererProvider::TAARendererProvider(int32_t width, int32_t height) {}
const char* TAARenderer::TAARendererProvider::GetShaderName()
{
	return "TAA";
}
void TAARenderer::TAARendererProvider::PatchPipelineState(Renderer::Pipeline* refToPS, uint32_t definesFlags) {

	refToPS->bs = &BlendStates::NoAlpha;
	refToPS->dss = &DepthStencilStates::NoDSS;
	refToPS->rs = &RasterizerStates::All;

}
const D3D11_INPUT_ELEMENT_DESC DefaultInputElements[] =
{
		{"Position",  0, DXGI_FORMAT_R32G32_FLOAT,  0,                            0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
};


Renderer::InputLayoutDescription TAARenderer::TAARendererProvider::GetInputLayoutDescription(size_t definesFlags) {
	return Renderer::InputLayoutDescription{ (void*)DefaultInputElements, std::size(DefaultInputElements) };
}


TAARenderer::TAARenderer() : QuadRenderer("TAAShader.hlsl") {}

void TAARenderer::Init(void* shaderData, size_t dataSize) {
	if (provider != nullptr) {
		//delete provider;
		delete factory;
		int32_t width, height;
		renderer->GetBackbufferSize(&width, &height);
		provider = new TAARendererProvider(width, height);
		factory = new TAARendererFactory(provider, shaderData, dataSize);
		return;
	}
	QuadRenderer::Init();
	provider = new TAARendererProvider(width, height);
	factory = new TAARendererFactory(provider, shaderData, dataSize);
	


	
	/*
	localBuffer.depthThreshold  = 0.0001;
	localBuffer.DilationMode = 1;
	localBuffer.ReprojectionMode = 1;
	localBuffer.NeighborhoodClampMode = 1;
	*/
	
	constBuffer = renderer->CreateConstBuffer(sizeof(localBuffer));
}

void TAARenderer::Render(GraphicsBase& gfx) {
	
}


void TAARenderer::UpdateHaltonSequence()
{
	if (localBuffer.numSamples == 0) {
		localBuffer.numSamples = 10;
	}

	if (HaltonSequence.size() < localBuffer.numSamples)
	{
		int32_t width, height;
		renderer->GetBackbufferSize(&width, &height);
		
		HaltonSequence.resize(localBuffer.numSamples);
		auto left = halton_sequence(20, 20 + localBuffer.numSamples-1, 2);
		//auto right = halton_sequence(20, 20 + localBuffer.numSamples-1, 3);

		for (int i = 0; i < localBuffer.numSamples*2; i+=2)
		{
			HaltonSequence[i/2] = float2(left[i], left[i+1]);
			HaltonSequence[i/2] = (float2(left[i], left[i+1])-float2(0.5,0.5)) / float2(width,height);
		}
		delete[] left;
		//delete[] right;
	}
	HaltonIndex = (HaltonIndex + 1) %localBuffer.numSamples;
}

void TAARenderer::RenderIMGUI(GraphicsBase& gfx)
{	

}


void TAARenderer::Clear(GraphicsBase& gfx)
{
	

	RenderTargetBinding* targets[] = {
		&gfx.texturesManger.velocityFieldRT
   };

	renderer->SetRenderTargets(targets, std::size(targets), gfx.texturesManger.depthBuffer, Viewport());
	renderer->Clear(ClearOptions::CLEAROPTIONS_TARGET, { 0, 0, 0, 0 }, 0, 0);
}

TAARenderer::~TAARenderer()
{
	renderer->AddDisposeConstBuffer(constBuffer);
	delete factory;
}
