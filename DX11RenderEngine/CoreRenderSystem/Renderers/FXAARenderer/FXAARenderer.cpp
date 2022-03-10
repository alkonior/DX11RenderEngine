#include "FXAARenderer.h"

#include "imgui/imgui.h"

using namespace Renderer;

FXAARenderer::FXAARendererProvider::FXAARendererProvider(int32_t width, int32_t height) {}

void FXAARenderer::FXAARendererProvider::PatchPipelineState(Renderer::PipelineState* refToPS, size_t definesFlags) {

	refToPS->bs = &BlendStates::NoAlpha;

	refToPS->dss.depthBufferEnable = false;
	refToPS->dss.depthBufferFunction = CompareFunction::COMPAREFUNCTION_LESSEQUAL;
	refToPS->dss.stencilEnable = false;
	refToPS->dss.depthBufferWriteEnable = false;

	refToPS->rs.cullMode = CullMode::CULLMODE_NONE;
	refToPS->rs.depthBias = 0.0f;
	refToPS->rs.fillMode = FillMode::FILLMODE_SOLID;
	refToPS->rs.multiSampleAntiAlias = 0;
	refToPS->rs.scissorTestEnable = 0;
	refToPS->rs.slopeScaleDepthBias = 0.0f;


}
const D3D11_INPUT_ELEMENT_DESC DefaultInputElements[] =
{
		{"Position",  0, DXGI_FORMAT_R32G32_FLOAT,  0,                            0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
};


Renderer::InputLayoutDescription FXAARenderer::FXAARendererProvider::GetInputLayoutDescription(size_t definesFlags) {
	return Renderer::InputLayoutDescription{ (void*)DefaultInputElements, std::size(DefaultInputElements) };
}

FXAARenderer::FXAARendererProvider::~FXAARendererProvider() {}

FXAARenderer::FXAARenderer():renderer(IRenderer::renderer) {}

void FXAARenderer::Init(void* shaderData, size_t dataSize) {
	if (provider != nullptr) {
		//delete provider;
		delete factory;
		int32_t width, height;
		renderer->GetBackbufferSize(&width, &height);
		provider = new FXAARendererProvider(width, height);
		factory = new FXAARendererFactory(renderer, provider, shaderData, dataSize);
		return;
	}

	int32_t width, height;
	renderer->GetBackbufferSize(&width, &height);
	provider = new FXAARendererProvider(width, height);
	factory = new FXAARendererFactory(renderer, provider, shaderData, dataSize);

	struct VertexFXAA
	{
		float2 pos;
		float2 uv;
	};

	VertexFXAA vertices[] =
	{
		{  float2(-1.0f, 1.0f),  float2(0.0f, 0.0f), },
		{  float2(-1.0f, -1.0f),  float2(0.0f, 1.0f), },
		{  float2(1.0f,  1.0f),  float2(1.0f, 0.0f), },
		{  float2(1.0f,  -1.0f),  float2(1.0f, 1.0f), }
	};
	vertexBuffer.buffersCount = 1;
	vertexBuffer.vertexBuffers = new Buffer * [1]();
	vertexBuffer.vertexBuffers[0] = renderer->GenVertexBuffer(0, BufferUsage::BUFFERUSAGE_NONE, sizeof(vertices));
	renderer->SetVertexBufferData(vertexBuffer.vertexBuffers[0], 0, &vertices, 4, sizeof(VertexFXAA), sizeof(VertexFXAA), SetDataOptions::SETDATAOPTIONS_NONE);
	//GFX_THROW_INFO(gfx.pDevice->CreateBuffer(&bd, &sd, &pVertexBuffer));
	vertexBuffer.vertexOffset = new (UINT)(0);
	vertexBuffer.vertexStride = new (UINT)(sizeof(VertexFXAA));
	// Bind vertex buffer to pipeline


	// create index buffer
	const uint16_t indices[] =
	{
		0,1,2,3
	};
	indexBuffer = renderer->GenIndexBuffer(0, BufferUsage::BUFFERUSAGE_WRITEONLY, 12);
	renderer->SetIndexBufferData(indexBuffer, 0, (void*)indices, 12, SetDataOptions::SETDATAOPTIONS_DISCARD);


	vp.x = 0;
	vp.y = 0;
	vp.w = width;
	vp.h = height;
	vp.minDepth = 0.0f;
	vp.maxDepth = 1.0f;

	sampler.filter = TextureFilter::TEXTUREFILTER_POINT;
	sampler.addressU = TextureAddressMode::TEXTUREADDRESSMODE_WRAP;
	sampler.addressV = TextureAddressMode::TEXTUREADDRESSMODE_WRAP;
	sampler.addressW = TextureAddressMode::TEXTUREADDRESSMODE_WRAP;
	
	diffuseSampler.filter = TextureFilter::TEXTUREFILTER_ANISOTROPIC;
	diffuseSampler.addressU = TextureAddressMode::TEXTUREADDRESSMODE_WRAP;
	diffuseSampler.addressV = TextureAddressMode::TEXTUREADDRESSMODE_WRAP;
	diffuseSampler.addressW = TextureAddressMode::TEXTUREADDRESSMODE_WRAP;
	diffuseSampler.maxAnisotropy = 16;

	localBuffer.fxaaQualitySubpix			= 0.75;
	localBuffer.fxaaQualityEdgeThreshold	= 0.166;
	localBuffer.fxaaQualityEdgeThresholdMin	= 0.0833;
	localBuffer.fxaaConsoleEdgeSharpness	= 8.0;
	localBuffer.fxaaConsoleEdgeThreshold	= 0.125;
	localBuffer.fxaaConsoleEdgeThresholdMin	= 0.05;
	
	constBuffer = renderer->CreateConstBuffer(sizeof(localBuffer));
}

void FXAARenderer::Init(LPCWSTR dirr) {}

void FXAARenderer::Render(GraphicsBase& gfx) {
	int32_t width, height;
	renderer->GetBackbufferSize(&width, &height);

	RenderIMGUI(gfx);
	
	//uint64_t flags = 0;
	//if (lightOnly)
	//	flags|=ENDLIGHTONLY;
	//if (colorOnly)
	//	flags|=ENDCOLORONLY;
	//if (bloomOnly)
	//	flags|=ENDBLOOMONLY;
	//if (alphaOnly)
	//	flags|=ENDALPHAONLY;
	
	gfx.texturesManger.CreateRenderTarget(SURFACEFORMAT_COLOR, width, height, preFXAA, preFXAART);

	
	renderer->ApplyVertexBufferBinding(vertexBuffer);
	renderer->ApplyIndexBufferBinding(indexBuffer, 16);

	renderer->VerifyConstBuffer(constBuffer, FXAABuffer.slot);
	renderer->SetConstBuffer(constBuffer, &localBuffer);

	
	RenderTargetBinding* target[1] = {&preFXAART};
	renderer->SetRenderTargets(target, 1, nullptr, DepthFormat::DEPTHFORMAT_D32, vp);
	renderer->VerifyPixelSampler(0, sampler);
	renderer->VerifyPixelTexture(0, gfx.texturesManger.preFXAAcolor);

	renderer->ApplyPipelineState(factory->GetState(FXAASETLUMA));
	renderer->DrawIndexedPrimitives(PrimitiveType::PRIMITIVETYPE_TRIANGLESTRIP, 0, 0, 0, 0, 2);


	
	renderer->SetRenderTargets(nullptr, 0, nullptr, DepthFormat::DEPTHFORMAT_D32, vp);
	renderer->VerifyPixelSampler(0, diffuseSampler);
	renderer->VerifyPixelTexture(0, preFXAA);
	renderer->ApplyPipelineState(factory->GetState(FXAAZERO));
	renderer->DrawIndexedPrimitives(PrimitiveType::PRIMITIVETYPE_TRIANGLESTRIP, 0, 0, 0, 0, 2);

	

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
	delete vertexBuffer.vertexOffset;
	delete vertexBuffer.vertexStride;
	renderer->AddDisposeVertexBuffer(vertexBuffer.vertexBuffers[0]);
	delete[] vertexBuffer.vertexBuffers;
	renderer->AddDisposeIndexBuffer(indexBuffer);
	renderer->AddDisposeConstBuffer(constBuffer);
	//renderer->AddDisposeConstBuffer(constBuffer);
	//delete provider;
	delete factory;
}
