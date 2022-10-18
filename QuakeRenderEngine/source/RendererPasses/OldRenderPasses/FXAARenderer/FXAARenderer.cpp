#include "pch.h"
#include "FXAARenderer.h"

#include <d3d11.h>

#include "imgui/imgui.h"

using namespace Renderer;

FXAARenderer::FXAARendererProvider::FXAARendererProvider(int32_t width, int32_t height) {}
const char* FXAARenderer::FXAARendererProvider::GetShaderName()
{
	return "FXAA";
}
void FXAARenderer::FXAARendererProvider::PatchPipelineState(Renderer::PipelineState* refToPS, size_t definesFlags) {

	refToPS->bs = &BlendStates::NoAlpha;
	refToPS->dss = &DepthStencilStates::NoDSS;
	refToPS->rs = &RasterizerStates::All;

}
const D3D11_INPUT_ELEMENT_DESC DefaultInputElements[] =
{
		{"Position",  0, DXGI_FORMAT_R32G32_FLOAT,  0,                            0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
};


Renderer::InputLayoutDescription FXAARenderer::FXAARendererProvider::GetInputLayoutDescription(size_t definesFlags) {
	return Renderer::InputLayoutDescription{ (void*)DefaultInputElements, std::size(DefaultInputElements) };
}


FXAARenderer::FXAARenderer() : QuadRenderer("FXAAShader.hlsl") {}

void FXAARenderer::Init(void* shaderData, size_t dataSize) {

	QuadRenderer::Init();
	
	int32_t width, height;
	renderer->GetBackbufferSize(&width, &height);
	provider = new FXAARendererProvider(width, height);
	factory = new FXAARendererFactory(provider, shaderData, dataSize);
	

	localBuffer.fxaaQualitySubpix			= 0.75;
	localBuffer.fxaaQualityEdgeThreshold	= 0.166;
	localBuffer.fxaaQualityEdgeThresholdMin	= 0.0833;
	localBuffer.fxaaConsoleEdgeSharpness	= 8.0;
	localBuffer.fxaaConsoleEdgeThreshold	= 0.125;
	localBuffer.fxaaConsoleEdgeThresholdMin	= 0.05;
	
	constBuffer = renderer->CreateConstBuffer(sizeof(localBuffer));
}

void FXAARenderer::Render(GraphicsBase& gfx) {
	QuadRenderer::Render();
	int32_t width, height;
	renderer->GetBackbufferSize(&width, &height);

	RenderIMGUI(gfx);
	
	gfx.texturesManger.CreateRenderTarget(SURFACEFORMAT_COLOR, width, height, preFXAA, preFXAART);

	renderer->VerifyConstBuffer(constBuffer, FXAABuffer.slot);
	renderer->SetConstBuffer(constBuffer, &localBuffer);

	
	RenderTargetBinding* target[1] = {&preFXAART};
	renderer->SetRenderTargets(target, 1, nullptr, vp);
	renderer->VerifyPixelSampler(0, Samplers::anisotropic16);
	renderer->VerifyPixelTexture(0, gfx.texturesManger.preAAcolor);

	renderer->ApplyPipelineState(factory->GetState(FXAASETLUMA));
	renderer->DrawIndexedPrimitives(Renderer::PrimitiveType::PRIMITIVETYPE_TRIANGLESTRIP, 0, 0, 0, 0, 2);


	
	renderer->SetRenderTargets(nullptr, 0, nullptr, vp);
	renderer->VerifyPixelSampler(0, Samplers::anisotropic16);
	renderer->VerifyPixelTexture(0, preFXAA);
	renderer->ApplyPipelineState(factory->GetState(FXAAZERO));
	renderer->DrawIndexedPrimitives(Renderer::PrimitiveType::PRIMITIVETYPE_TRIANGLESTRIP, 0, 0, 0, 0, 2);
}


void FXAARenderer::RenderIMGUI(GraphicsBase& gfx)
{	
	ImGui::Begin("FXAA settings.");                          // Create a window called "Hello, world!" and append into it.
//
	ImGui::SliderFloat("fxaaQualitySubpix",           &localBuffer.fxaaQualitySubpix			,0.0, 1.0 , "%.3f");
	ImGui::SliderFloat("fxaaQualityEdgeThreshold",    &localBuffer.fxaaQualityEdgeThreshold		,0.0, 1.0 , "%.3f");
	ImGui::SliderFloat("fxaaQualityEdgeThresholdMin", &localBuffer.fxaaQualityEdgeThresholdMin	,0.0, 1.0 , "%.3f");
	ImGui::SliderFloat("fxaaConsoleEdgeSharpness",    &localBuffer.fxaaConsoleEdgeSharpness		,0.0, 10.0, "%.3f");
	ImGui::SliderFloat("fxaaConsoleEdgeThreshold",    &localBuffer.fxaaConsoleEdgeThreshold		,0.0, 1.0 , "%.3f");
	ImGui::SliderFloat("fxaaConsoleEdgeThresholdMin", &localBuffer.fxaaConsoleEdgeThresholdMin	,0.0, 1.0 , "%.3f");
	
	ImGui::End();
}


FXAARenderer::~FXAARenderer()
{
	renderer->AddDisposeConstBuffer(constBuffer);
	delete factory;
}
