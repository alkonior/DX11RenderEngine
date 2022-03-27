#include "TAARenderer.h"

#include "imgui/imgui.h"
#include "halton.h"

using namespace Renderer;

TAARenderer::TAARendererProvider::TAARendererProvider(int32_t width, int32_t height) {}
const char* TAARenderer::TAARendererProvider::GetShaderName()
{
	return "TAA";
}
void TAARenderer::TAARendererProvider::PatchPipelineState(Renderer::PipelineState* refToPS, size_t definesFlags) {

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
	
	int32_t width, height;
	renderer->GetBackbufferSize(&width, &height);
	provider = new TAARendererProvider(width, height);
	factory = new TAARendererFactory(provider, shaderData, dataSize);
	

	localBuffer.numSamples		= 10;
	localBuffer.depthThreshold  = 0.0001;
	
	constBuffer = renderer->CreateConstBuffer(sizeof(localBuffer));
}

void TAARenderer::Render(GraphicsBase& gfx) {
	QuadRenderer::Render();
	int32_t width, height;
	renderer->GetBackbufferSize(&width, &height);

	RenderIMGUI(gfx);
	
	renderer->VerifyConstBuffer(constBuffer, TAABuffer.slot);
	renderer->SetConstBuffer(constBuffer, &localBuffer);
	
	gfx.texturesManger.CreateRenderTarget(SURFACEFORMAT_COLOR, width, height, TAAresult, TAAresultRT);

	
	RenderTargetBinding* target[3] = {
		&TAAresultRT,
		&gfx.texturesManger.pastColorRT,
		&gfx.texturesManger.pastDepthRT,
	};
	
	
	renderer->SetRenderTargets(target, 1, nullptr, vp);
	renderer->VerifyPixelSampler(0, Samplers::point);
	renderer->VerifyPixelTexture(0, gfx.texturesManger.preAAcolor);
	renderer->VerifyPixelTexture(1, gfx.texturesManger.depthBuffer->texture);
	renderer->VerifyPixelTexture(2, gfx.texturesManger.pastColor);
	renderer->VerifyPixelTexture(3, gfx.texturesManger.pastDepth);
	renderer->VerifyPixelTexture(4, gfx.texturesManger.velocityField);

	renderer->ApplyPipelineState(factory->GetState(TAAZERO));
	renderer->DrawIndexedPrimitives(PrimitiveType::PRIMITIVETYPE_TRIANGLESTRIP, 0, 0, 0, 0, 2);


	target[0] = nullptr;
	renderer->SetRenderTargets(target, 3, nullptr, vp);
	renderer->VerifyPixelSampler(0, Samplers::point);
	renderer->VerifyPixelTexture(0, TAAresult);
	renderer->ApplyPipelineState(factory->GetState(TAACOPY));
	renderer->DrawIndexedPrimitives(PrimitiveType::PRIMITIVETYPE_TRIANGLESTRIP, 0, 0, 0, 0, 2);

	renderer->VerifyPixelTexture(1, nullptr);
}


void TAARenderer::UpdateHaltonSequence()
{
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
	ImGui::Begin("TAA settings.");                          // Create a window called "Hello, world!" and append into it.
//
	ImGui::SliderFloat("depthThreshold",     &localBuffer.depthThreshold ,0.0, 1.0 , "%.10f");
	ImGui::SliderInt("NumSamples",				&localBuffer.numSamples , 1, 20 , "%.3f");
	ImGui::SliderFloat("TAAShiftStrength",     &gfx.taaConstants.taaStrength, 0.0, 100.0  , "%.3f");
	//ImGui::SliderFloat("fxaaQualityEdgeThreshold"   ,    &localBuffer.fxaaQualityEdgeThreshold		,0.0, 1.0 , "%.3f");
	//ImGui::SliderFloat("fxaaQualityEdgeThresholdMin", &localBuffer.fxaaQualityEdgeThresholdMin	,0.0, 1.0 , "%.3f");


	ImGui::End();
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