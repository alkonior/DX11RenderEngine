#include "BloomRenderer.h"

#include "imgui/imgui.h"

using namespace Renderer;

BloomRenderer::BloomRendererProvider::BloomRendererProvider(int32_t width, int32_t height) {}

void BloomRenderer::BloomRendererProvider::PatchPipelineState(Renderer::PipelineState* refToPS, size_t definesFlags) {

	refToPS->bs = &BlendStates::NoAlpha;
	refToPS->dss = &DepthStencilStates::NoDSS;
	refToPS->rs = &RasterizerStates::All;

}

const D3D11_INPUT_ELEMENT_DESC DefaultInputElements[] =
{
	{"Position",  0, DXGI_FORMAT_R32G32_FLOAT,  0,                            0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
};


InputLayoutDescription BloomRenderer::BloomRendererProvider::GetInputLayoutDescription(size_t definesFlags) {
	return Renderer::InputLayoutDescription{ (void*)DefaultInputElements, std::size(DefaultInputElements) };
}

const char* BloomRenderer::BloomRendererProvider::GetShaderName()
{
	return "Bloom shader";
}


BloomRenderer::BloomRenderer() : QuadRenderer("BloomShader.hlsl") {}

void BloomRenderer::Init(void* shaderData, size_t dataSize) {
	if (provider != nullptr) {
		//delete provider;
		delete factory;
		int32_t width, height;
		renderer->GetBackbufferSize(&width, &height);
		provider = new BloomRendererProvider(width, height);
		factory = new BloomRendererFactory(provider, shaderData, dataSize);
		return;
	}
	struct VertexEnd
	{
		float2 pos;
		float2 uv;
	};

	VertexEnd vertices[] =
		{
		{  float2(-1.0f, 1.0f),  float2(0.0f, 0.0f), },
		{  float2(-1.0f, -1.0f),  float2(0.0f, 1.0f), },
		{  float2(1.0f,  1.0f),  float2(1.0f, 0.0f), },
		{  float2(1.0f,  -1.0f),  float2(1.0f, 1.0f), }
		};
	
	QuadRenderer::Init();
	
	int32_t width, height;
	renderer->GetBackbufferSize(&width, &height);
	provider = new BloomRendererProvider(width, height);
	factory = new BloomRendererFactory(provider, shaderData, dataSize);

	sampler.filter = TextureFilter::TEXTUREFILTER_POINT;
	sampler.addressU = TextureAddressMode::TEXTUREADDRESSMODE_WRAP;
	sampler.addressV = TextureAddressMode::TEXTUREADDRESSMODE_WRAP;
	sampler.addressW = TextureAddressMode::TEXTUREADDRESSMODE_WRAP;

	constBuffer = renderer->CreateConstBuffer(sizeof(localBuffer));
	
	localBuffer.intensity = 5;
	localBuffer.radius =		9;
	localBuffer.sigma =			4.7;
	localBuffer.threshold =	  0.45;
	kernel = 4;
}


void BloomRenderer::RenderBloomMask(GraphicsBase& gfx)
{
	QuadRenderer::Render();
	

	RenderTargetBinding* targets[1];
	
	renderer->VerifyConstBuffer(constBuffer, BloomCosntants.slot);
	renderer->SetConstBuffer(constBuffer, &localBuffer);
	
	targets[0] = &gfx.texturesManger.bloomMaskRT;
	
	renderer->ApplyPipelineState(factory->GetState(BLOOMTHRESHOLD));
	renderer->SetRenderTargets(targets, 1, nullptr, Viewport());
	renderer->VerifyPixelSampler(0, Samplers::pointClamp);
	renderer->VerifyPixelTexture(0, gfx.texturesManger.diffuseColor);
	renderer->DrawIndexedPrimitives(PrimitiveType::PRIMITIVETYPE_TRIANGLESTRIP, 0, 0, 0, 0, 2);
	
}
void BloomRenderer::Render(GraphicsBase& gfx) {
	QuadRenderer::Render();
	
	int32_t width, height;
	renderer->GetBackbufferSize(&width, &height);

	RenderTargetBinding* targets[3];

	gfx.texturesManger.CreateRenderTarget(SURFACEFORMAT_COLOR, width, height, bloom1, bloom1RT);
	gfx.texturesManger.CreateRenderTarget(SURFACEFORMAT_COLOR, width, height, bloom2, bloom2RT);

	RenderIMGUI(gfx);
	
	renderer->VerifyConstBuffer(constBuffer, BloomCosntants.slot);
	//renderer->SetConstBuffer(constBuffer, &localBuffer);
	
	targets[0] = &bloom1RT;
	renderer->ApplyPipelineState(factory->GetState(BLOOMTHRESHOLD));
	renderer->SetRenderTargets(targets, 1, nullptr, Viewport());
	renderer->VerifyPixelSampler(0, Samplers::pointClamp);
	renderer->VerifyPixelTexture(0, gfx.texturesManger.diffuseColor);
	renderer->DrawIndexedPrimitives(PrimitiveType::PRIMITIVETYPE_TRIANGLESTRIP, 0, 0, 0, 0, 2);

	targets[0] = &bloom2RT;
	targets[1] = &bloom1RT;
	for (int i = 0; i < kernel; i++)
	{
		renderer->SetRenderTargets(targets, 1, nullptr, Viewport());
		renderer->VerifyPixelTexture(0, targets[1]->texture);
		
		renderer->ApplyPipelineState(factory->GetState(BLOOMVERTICAL));
		renderer->DrawIndexedPrimitives(PrimitiveType::PRIMITIVETYPE_TRIANGLESTRIP, 0, 0, 0, 0, 2);
		renderer->VerifyPixelTexture(0, nullptr);
		
		targets[2] = targets[0];
		targets[0] = targets[1];
		targets[1] = targets[2];

		renderer->SetRenderTargets(targets, 1, nullptr, Viewport());
		renderer->VerifyPixelTexture(0, targets[1]->texture);

		renderer->ApplyPipelineState(factory->GetState(BLOOMHORISONTAL));
		renderer->DrawIndexedPrimitives(PrimitiveType::PRIMITIVETYPE_TRIANGLESTRIP, 0, 0, 0, 0, 2);
		renderer->VerifyPixelTexture(0, nullptr);
		
		targets[2] = targets[0];
		targets[0] = targets[1];
		targets[1] = targets[2];
	}

	targets[0] = &gfx.texturesManger.bloomMaskRT;
	
	renderer->SetRenderTargets(targets, 1, nullptr, Viewport());
	renderer->ApplyPipelineState(factory->GetState(BLOOMEND));
	renderer->VerifyPixelTexture(0, targets[1]->texture);

	renderer->DrawIndexedPrimitives(PrimitiveType::PRIMITIVETYPE_TRIANGLESTRIP, 0, 0, 0, 0, 2);
	
}

BloomRenderer::~BloomRenderer(){
	renderer->AddDisposeConstBuffer(constBuffer);
	//delete provider;
	delete factory;
}

void BloomRenderer::RenderIMGUI(GraphicsBase& gfx)
{	
	ImGui::Begin("Bloom settings.");                          // Create a window called "Hello, world!" and append into it.
          
	ImGui::SliderFloat("Intensity.", &localBuffer.intensity, 0.0f, 10.0f);
	
	ImGui::SliderInt("Radius.", &localBuffer.radius, 0, 10);
	       
	ImGui::SliderFloat("Sigma.", &localBuffer.sigma, 0.01f, 5.0f);
	
	ImGui::SliderFloat("Threshold.", &localBuffer.threshold, 0.0f, 1.0);
	
	ImGui::SliderInt("Kernel.", &kernel, 1, 10);
	
	ImGui::End();
}
