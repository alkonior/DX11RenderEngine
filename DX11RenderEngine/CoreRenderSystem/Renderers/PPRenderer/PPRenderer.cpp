#include "PPRenderer.h"

#include "imgui/imgui.h"

using namespace Renderer;

PPRenderer::PPRendererProvider::PPRendererProvider(int32_t width, int32_t height) {}

void PPRenderer::PPRendererProvider::PatchPipelineState(Renderer::PipelineState* refToPS, size_t definesFlags) {

	refToPS->bs = &BlendStates::NoAlpha;
	
	if (definesFlags & (PPALPHA)) {
		refToPS->bs = &BlendStates::Alpha;
	}

	refToPS->dss = &DepthStencilStates::NoDSS;
	refToPS->rs = &RasterizerStates::All;

}
const D3D11_INPUT_ELEMENT_DESC DefaultInputElements[] =
{
		{"Position",  0, DXGI_FORMAT_R32G32_FLOAT,  0,                            0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
};


Renderer::InputLayoutDescription PPRenderer::PPRendererProvider::GetInputLayoutDescription(size_t definesFlags) {
	return Renderer::InputLayoutDescription{ (void*)DefaultInputElements, std::size(DefaultInputElements) };
}

const char* PPRenderer::PPRendererProvider::GetShaderName()
{
	return "PostProcess";
}


PPRenderer::PPRenderer():QuadRenderer("PostProcessShader.hlsl") {}

void PPRenderer::Init(void* shaderData, size_t dataSize) {
	if (provider != nullptr) {
		//delete provider;
		delete factory;
		int32_t width, height;
		renderer->GetBackbufferSize(&width, &height);
		provider = new PPRendererProvider(width, height);
		factory = new PPRendererFactory(provider, shaderData, dataSize);
		return;
	}
	QuadRenderer::Init();
	int32_t width, height;
	renderer->GetBackbufferSize(&width, &height);
	provider = new PPRendererProvider(width, height);
	factory = new PPRendererFactory(provider, shaderData, dataSize);

	sampler.filter = TextureFilter::TEXTUREFILTER_POINT;
	sampler.addressU = TextureAddressMode::TEXTUREADDRESSMODE_WRAP;
	sampler.addressV = TextureAddressMode::TEXTUREADDRESSMODE_WRAP;
	sampler.addressW = TextureAddressMode::TEXTUREADDRESSMODE_WRAP;

	diffuseSampler.filter = TextureFilter::TEXTUREFILTER_ANISOTROPIC;
	diffuseSampler.addressU = TextureAddressMode::TEXTUREADDRESSMODE_WRAP;
	diffuseSampler.addressV = TextureAddressMode::TEXTUREADDRESSMODE_WRAP;
	diffuseSampler.addressW = TextureAddressMode::TEXTUREADDRESSMODE_WRAP;
	diffuseSampler.maxAnisotropy = 16;

	pConstBuffer = renderer->CreateConstBuffer(sizeof(localData));
	localData.lightAdd = 0.3;
	//localData.numSampes = 1;
}

void PPRenderer::Init(LPCWSTR dirr) {}

void PPRenderer::Render(GraphicsBase& gfx) {
	QuadRenderer::Render();
	RenderIMGUI(gfx);
	
	uint64_t flags = 0;
	if (lightOnly)
		flags|=PPLIGHTONLY;
	if (colorOnly)
		flags|=PPCOLORONLY;
	if (bloomOnly)
		flags|=PPBLOOMONLY;
	if (alphaOnly)
		flags|=PPALPHAONLY;
	if (occlusionOnly)
		flags|=PPOCCLUSIONONLY;
	

	RenderTargetBinding* targets[1] = {&gfx.texturesManger.preAAcolorRT};
	renderer->SetRenderTargets(targets, 1, nullptr, vp);
	
	renderer->VerifyPixelSampler(0, Samplers::pointClamp);

	renderer->VerifyPixelTexture(0, gfx.texturesManger.diffuseColor);
	renderer->VerifyPixelTexture(1, gfx.texturesManger.bloomMask);
	renderer->VerifyPixelTexture(2, gfx.texturesManger.lightColor);
	renderer->VerifyPixelTexture(3, gfx.texturesManger.depthBuffer->texture);
	renderer->VerifyPixelTexture(4, gfx.texturesManger.alphaSurfaces);
	renderer->VerifyPixelTexture(5, gfx.texturesManger.oclusionField);
	
	renderer->VerifyConstBuffer(pConstBuffer, ppCosntBuffer.slot);
	renderer->SetConstBuffer(pConstBuffer, &localData);

	renderer->ApplyPipelineState(factory->GetState(PPZERO | flags));
	renderer->DrawIndexedPrimitives(PrimitiveType::PRIMITIVETYPE_TRIANGLESTRIP, 0, 0, 0, 0, 2);

	renderer->VerifyPixelTexture(5, nullptr);

	
	renderer->ApplyPipelineState(factory->GetState(PPALPHA | flags));
	renderer->DrawIndexedPrimitives(PrimitiveType::PRIMITIVETYPE_TRIANGLESTRIP, 0, 0, 0, 0, 2);

	
	renderer->VerifyPixelTexture(3, nullptr);
}


void PPRenderer::RenderIMGUI(GraphicsBase& gfx)
{	
	ImGui::Begin("PostProcess settings.");                          // Create a window called "Hello, world!" and append into it.

	ImGui::Checkbox("ColorOnly", &colorOnly);
	ImGui::Checkbox("LightOnly", &lightOnly);
	ImGui::Checkbox("BloomOnly", &bloomOnly);
	ImGui::Checkbox("AlphaOnly", &alphaOnly);
	ImGui::Checkbox("BlureOnly", &blureOnly);
	ImGui::Checkbox("OcclusionOnly", &occlusionOnly);
	//ImGui::SliderInt("MotionBlure samples", &localData.numSampes, 1,10);
	ImGui::SliderFloat("light Add", &localData.lightAdd, 0, 1.0);
	ImGui::End();
}


PPRenderer::~PPRenderer()
{
	delete factory;
}
