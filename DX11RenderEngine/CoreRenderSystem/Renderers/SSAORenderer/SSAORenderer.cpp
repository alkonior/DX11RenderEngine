#include "SSAORenderer.h"

#include "imgui/imgui.h"
#include "halton.h"

using namespace Renderer;

SSAORenderer::SSAORendererProvider::SSAORendererProvider(int32_t width, int32_t height) {}
const char* SSAORenderer::SSAORendererProvider::GetShaderName()
{
	return "SSAO";
}

void SSAORenderer::SSAORendererProvider::PatchPipelineState(Renderer::PipelineState* refToPS, size_t definesFlags) {

	refToPS->bs = &BlendStates::NoAlpha;
	refToPS->dss = &DepthStencilStates::NoDSS;
	refToPS->rs = &RasterizerStates::All;

}
const D3D11_INPUT_ELEMENT_DESC DefaultInputElements[] =
{
		{"Position",  0, DXGI_FORMAT_R32G32_FLOAT,  0,                            0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
};


Renderer::InputLayoutDescription SSAORenderer::SSAORendererProvider::GetInputLayoutDescription(size_t definesFlags) {
	return Renderer::InputLayoutDescription{ (void*)DefaultInputElements, std::size(DefaultInputElements) };
}


SSAORenderer::SSAORenderer() : QuadRenderer("SSAOShader.hlsl") {}

float RandF()
{
	return (float)(rand()) / (float)RAND_MAX;
}

// Returns random float in [a, b).
float RandF(float a, float b)
{
	return a + RandF()*(b-a);
}

void SSAORenderer::Init(void* shaderData, size_t dataSize) {
	if (provider != nullptr) {
		//delete provider;
		delete factory;
		int32_t width, height;
		renderer->GetBackbufferSize(&width, &height);
		provider = new SSAORendererProvider(width, height);
		factory = new SSAORendererFactory(provider, shaderData, dataSize);
		return;
	}
	QuadRenderer::Init();
	
	int32_t width, height;
	renderer->GetBackbufferSize(&width, &height);
	provider = new SSAORendererProvider(width, height);
	factory = new SSAORendererFactory(provider, shaderData, dataSize);
	

	randVect = renderer->CreateTexture2D(SURFACEFORMAT_VECTOR4,256, 256,1, 0);

	//double* randData = halton_sequence(10,256*256*10, 3);
	std::vector<float4> initData(256*256);
	
	for(int i = 0; i < 256; ++i)
	{
		for(int j = 0; j < 256; ++j)
		{
			initData[j * 256 + i] = float4(RandF(), RandF(), RandF(), 0);
		}
	}
	//delete[] randData;
	renderer->SetTextureData2D(randVect,0,0,256,256,0,initData.data(),256*256);

	float4 mOffsets[14];
	
	mOffsets[0] = float4(+1.0f, +1.0f, +1.0f, 0.0f);
	mOffsets[1] = float4(-1.0f, -1.0f, -1.0f, 0.0f);

	mOffsets[2] = float4(-1.0f, +1.0f, +1.0f, 0.0f);
	mOffsets[3] = float4(+1.0f, -1.0f, -1.0f, 0.0f);

	mOffsets[4] = float4(+1.0f, +1.0f, -1.0f, 0.0f);
	mOffsets[5] = float4(-1.0f, -1.0f, +1.0f, 0.0f);

	mOffsets[6] = float4(-1.0f, +1.0f, -1.0f, 0.0f);
	mOffsets[7] = float4(+1.0f, -1.0f, +1.0f, 0.0f);

	// 6 centers of cube faces
	mOffsets[8] =  float4(-1.0f, 0.0f, 0.0f, 0.0f);
	mOffsets[9] =  float4(+1.0f, 0.0f, 0.0f, 0.0f);

	mOffsets[10] = float4(0.0f, -1.0f, 0.0f, 0.0f);
	mOffsets[11] = float4(0.0f, +1.0f, 0.0f, 0.0f);

	mOffsets[12] = float4(0.0f, 0.0f, -1.0f, 0.0f);
	mOffsets[13] = float4(0.0f, 0.0f, +1.0f, 0.0f);

	for(int i = 0; i < 14; ++i)
	{
		// Create random lengths in [0.25, 1.0].
		float s = RandF(0.25f, 1.0f);
		mOffsets[i].Normalize();
		
		float4 v = s * mOffsets[i];

		localBuffer.gOffsetVectors[i] = v;
	}
	
	/*
	localBuffer.depthThreshold  = 0.0001;
	localBuffer.DilationMode = 1;
	localBuffer.ReprojectionMode = 1;
	localBuffer.NeighborhoodClampMode = 1;
	*/
	localBuffer.sigma = 1;
	localBuffer.kernel = 1;
	localBuffer.inencity = 1;
	localBuffer.gInvRenderTargetSize = float2(1.f/width,1.f/height);
	
	localBuffer.gOcclusionRadius   = 0.5;
	localBuffer.gOcclusionFadeStart = 0.2;
	localBuffer.gOcclusionFadeEnd   = 1.0;
	localBuffer.gSurfaceEpsilon   = 0.05;
	
	constBuffer = renderer->CreateConstBuffer(sizeof(localBuffer));
}

void SSAORenderer::Render(GraphicsBase& gfx) {
	QuadRenderer::Render();

	matrix T(
	0.5f, 0.0f, 0.0f, 0.0f,
	0.0f, -0.5f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.5f, 0.5f, 0.0f, 1.0f);
	localBuffer.gProjTex = (gfx.viewConstants.projection.Transpose()*T).Transpose();
	
	int32_t width, height;
	renderer->GetBackbufferSize(&width, &height);

	RenderIMGUI(gfx);
	

	size_t flags = SSAOZERO;
	
	RenderTargetBinding* target[] = {
		&gfx.texturesManger.oclusionFieldRT,
	};
	renderer->SetRenderTargets(target, 1, nullptr, vp);
	
	renderer->SetConstBuffer(constBuffer, &localBuffer);
	renderer->VerifyConstBuffer(constBuffer, SSAOData.slot);

	
	
	renderer->VerifyPixelSampler(0, Samplers::pointClamp);
	renderer->VerifyPixelSampler(1, Samplers::linearClamp);
	renderer->VerifyPixelSampler(2, Samplers::pointClamp);
	renderer->VerifyPixelSampler(3, Samplers::linearWrap);
	
	
	renderer->VerifyPixelTexture(0, gfx.texturesManger.normalsField);
	renderer->VerifyPixelTexture(1, gfx.texturesManger.depthBuffer->texture);
	renderer->VerifyPixelTexture(2, randVect);
	
	renderer->ApplyPipelineState(factory->GetState(flags));
	renderer->DrawIndexedPrimitives(PrimitiveType::PRIMITIVETYPE_TRIANGLELIST, 0, 0, 0, 0, 2);

	
	//renderer->Dispatch((width + SSAO_NUM_THREADS_X -1) / (SSAO_NUM_THREADS_X),
	////	(height + SSAO_NUM_THREADS_Y -1) / (SSAO_NUM_THREADS_Y));
	//renderer->VerifyUATexture(0, nullptr);
	//renderer->VerifyPixelTexture(0, nullptr);
	//renderer->VerifyPixelTexture(1, nullptr);
	//renderer->VerifyPixelTexture(2, nullptr);
	//renderer->VerifyPixelTexture(3, nullptr);
	//renderer->VerifyPixelTexture(4, nullptr);
//
	////target[0] = nullptr;
	//renderer->SetRenderTargets(target, 3, nullptr, vp);
	//renderer->VerifyPixelSampler(0, Samplers::pointClamp);
	//renderer->VerifyPixelTexture(0, SSAOHistory);
	//renderer->VerifyPixelTexture(1, gfx.texturesManger.depthBuffer->texture);
	//renderer->ApplyPipelineState(factory->GetState(SSAOCOPY));
	//renderer->DrawIndexedPrimitives(PrimitiveType::PRIMITIVETYPE_TRIANGLESTRIP, 0, 0, 0, 0, 2);
//
	//renderer->VerifyPixelTexture(1, nullptr);
	
}


void SSAORenderer::RenderIMGUI(GraphicsBase& gfx)
{	
	ImGui::Begin("SSAO settings.");                          // Create a window called "Hello, world!" and append into it.

	ImGui::SliderFloat("gOcclusionRadius  ",&localBuffer.gOcclusionRadius   ,0,3);
	ImGui::SliderFloat("gOcclusionFadeStar",&localBuffer.gOcclusionFadeStart,0,3);
	ImGui::SliderFloat("gOcclusionFadeEnd ",&localBuffer.gOcclusionFadeEnd  ,0,3);
	ImGui::SliderFloat("gSurfaceEpsilon   ",&localBuffer.gSurfaceEpsilon    ,0,3);

	ImGui::End();
}


void SSAORenderer::Clear(GraphicsBase& gfx)
{
	

	RenderTargetBinding* targets[] = {
		&gfx.texturesManger.velocityFieldRT
   };

	renderer->SetRenderTargets(targets, std::size(targets), gfx.texturesManger.depthBuffer, Viewport());
	renderer->Clear(ClearOptions::CLEAROPTIONS_TARGET, { 0, 0, 0, 0 }, 0, 0);
}

SSAORenderer::~SSAORenderer()
{
	renderer->AddDisposeConstBuffer(constBuffer);
	delete factory;
}
