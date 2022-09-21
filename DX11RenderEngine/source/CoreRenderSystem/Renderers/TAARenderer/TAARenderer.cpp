#include "pch.h"
#include "TAARenderer.h"

#include <d3d11.h>

#include "imgui/imgui.h"
#include "Utils/halton.h"

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
	

	TAAHistory = renderer->CreateUATexture2D(SURFACEFORMAT_VECTOR4,width, height,1);

	
	localBuffer.numSamples		= 10;
	UpdateHaltonSequence();
	localBuffer.Jitter = HaltonSequence[HaltonIndex];
	localBuffer.Resolution = float4(width, height, 1.f/width, 1.f/height);
	localBuffer.FrameNumber = HaltonIndex;
	
	/*
	localBuffer.depthThreshold  = 0.0001;
	localBuffer.DilationMode = 1;
	localBuffer.ReprojectionMode = 1;
	localBuffer.NeighborhoodClampMode = 1;
	*/
	
	constBuffer = renderer->CreateConstBuffer(sizeof(localBuffer));
}

void TAARenderer::Render(GraphicsBase& gfx) {
	QuadRenderer::Render();
	int32_t width, height;
	renderer->GetBackbufferSize(&width, &height);

	RenderIMGUI(gfx);
	

	size_t flags = TAAZERO;
	localBuffer.DebugFlags = 0;
	if (markNoHistoryPixels)
		localBuffer.DebugFlags |= MarkNoHistoryPixels;
	
	if (allowBicubicFilter)
		localBuffer.DebugFlags |= AllowBicubicFilter;
	
	if (allowDepthThreshold)
		localBuffer.DebugFlags |= AllowDepthThreshold;
	
	if (allowNeighbourhoodSampling)
		localBuffer.DebugFlags |= AllowNeighbourhoodSampling;
	
	if (allowVarianceClipping)
		localBuffer.DebugFlags |= AllowVarianceClipping;
	
	if (allowLongestVelocityVector)
		localBuffer.DebugFlags |= AllowLongestVelocityVector;
	
	if (allowYCoCg)
		localBuffer.DebugFlags |= allowYCoCg;
	
	
	renderer->SetConstBuffer(constBuffer, &localBuffer);
	renderer->VerifyConstBuffer(constBuffer, CBData.slot);
	
	renderer->VerifyUATexture(0, TAAHistory);
	renderer->VerifyPixelSampler(0, Samplers::pointClamp);
	renderer->VerifyPixelSampler(1, Samplers::linearClamp);
	
	renderer->SetRenderTargets(nullptr, 0, nullptr, vp);
	
	renderer->VerifyPixelTexture(0, gfx.texturesManger.velocityField);
	renderer->VerifyPixelTexture(1, gfx.texturesManger.preAAcolor);
	renderer->VerifyPixelTexture(2, gfx.texturesManger.pastColor);
	renderer->VerifyPixelTexture(3, gfx.texturesManger.pastDepth);
	renderer->VerifyPixelTexture(4, gfx.texturesManger.depthBuffer->texture);
	
	renderer->ApplyPipelineState(factory->GetComputeState(flags, "main"));
	renderer->Dispatch((width + TAA_NUM_THREADS_X -1) / (TAA_NUM_THREADS_X),
		(height + TAA_NUM_THREADS_Y -1) / (TAA_NUM_THREADS_Y));
	renderer->VerifyUATexture(0, nullptr);
	renderer->VerifyPixelTexture(0, nullptr);
	renderer->VerifyPixelTexture(1, nullptr);
	renderer->VerifyPixelTexture(2, nullptr);
	renderer->VerifyPixelTexture(3, nullptr);
	renderer->VerifyPixelTexture(4, nullptr);
	RenderTargetBinding* target[3] = {
		nullptr,
		&gfx.texturesManger.pastColorRT,
		&gfx.texturesManger.pastDepthRT,
	};
	//target[0] = nullptr;
	renderer->SetRenderTargets(target, 3, nullptr, vp);
	renderer->VerifyPixelSampler(0, Samplers::pointClamp);
	renderer->VerifyPixelTexture(0, TAAHistory);
	renderer->VerifyPixelTexture(1, gfx.texturesManger.depthBuffer->texture);
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
	//ImGui::SliderFloat("depthThreshold",     &localBuffer.depthThreshold ,0.0, 1.0 , "%.10f");
	ImGui::SliderInt("NumSamples",				&localBuffer.numSamples , 1, 20 , "%.3f");
	ImGui::SliderFloat("TAAShiftStrength",     &gfx.taaConstants.taaStrength, 0.0, 100.0  , "%.3f");
	ImGui::Checkbox("markNoHistoryPixels	    ", &markNoHistoryPixels		    );
	ImGui::Checkbox("allowBicubicFilter		    ", &allowBicubicFilter		    );
	ImGui::Checkbox("allowDepthThreshold	 	", &allowDepthThreshold		    );
	ImGui::Checkbox("allowVarianceClipping	    ", &allowVarianceClipping		);
	ImGui::Checkbox("allowNeighbourhoodSampling", &allowNeighbourhoodSampling	);
	ImGui::Checkbox("allowYCoCg				    ", &allowYCoCg					);
	ImGui::Checkbox("allowLongestVelocityVector", &allowLongestVelocityVector	);

	


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
