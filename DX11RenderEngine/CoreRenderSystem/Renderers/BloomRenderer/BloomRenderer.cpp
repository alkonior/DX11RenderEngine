#include "BloomRenderer.h"

#include "imgui/imgui.h"

using namespace Renderer;

BloomRenderer::BloomRendererProvider::BloomRendererProvider(int32_t width, int32_t height) {}

void BloomRenderer::BloomRendererProvider::PatchPipelineState(Renderer::PipelineState* refToPS, size_t definesFlags) {

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


InputLayoutDescription BloomRenderer::BloomRendererProvider::GetInputLayoutDescription(size_t definesFlags) {
	return Renderer::InputLayoutDescription{ (void*)DefaultInputElements, std::size(DefaultInputElements) };
}

BloomRenderer::BloomRendererProvider::~BloomRendererProvider() {}

BloomRenderer::BloomRenderer(Renderer::IRenderer* renderer) :renderer(renderer) {}

void BloomRenderer::Init(void* shaderData, size_t dataSize) {
	if (provider != nullptr) {
		//delete provider;
		delete factory;
		int32_t width, height;
		renderer->GetBackbufferSize(&width, &height);
		provider = new BloomRendererProvider(width, height);
		factory = new BloomRendererFactory(renderer, provider, shaderData, dataSize);
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
	vertexBuffer.buffersCount = 1;
	vertexBuffer.vertexBuffers = new Buffer * [1]();
	vertexBuffer.vertexBuffers[0] = renderer->GenVertexBuffer(0, BufferUsage::BUFFERUSAGE_NONE, sizeof(vertices));
	renderer->SetVertexBufferData(vertexBuffer.vertexBuffers[0], 0, &vertices, 4, sizeof(VertexEnd), sizeof(VertexEnd), SetDataOptions::SETDATAOPTIONS_NONE);
	//GFX_THROW_INFO(gfx.pDevice->CreateBuffer(&bd, &sd, &pVertexBuffer));
	vertexBuffer.vertexOffset = new (UINT)(0);
	vertexBuffer.vertexStride = new (UINT)(sizeof(VertexEnd));
	// Bind vertex buffer to pipeline


	// create index buffer
	const uint16_t indices[] =
	{
		0,1,2,3
	};
	indexBuffer = renderer->GenIndexBuffer(0, BufferUsage::BUFFERUSAGE_WRITEONLY, 12);
	renderer->SetIndexBufferData(indexBuffer, 0, (void*)indices, 12, SetDataOptions::SETDATAOPTIONS_DISCARD);

	
	int32_t width, height;
	renderer->GetBackbufferSize(&width, &height);
	provider = new BloomRendererProvider(width, height);
	factory = new BloomRendererFactory(renderer, provider, shaderData, dataSize);

	sampler.filter = TextureFilter::TEXTUREFILTER_POINT;
	sampler.addressU = TextureAddressMode::TEXTUREADDRESSMODE_WRAP;
	sampler.addressV = TextureAddressMode::TEXTUREADDRESSMODE_WRAP;
	sampler.addressW = TextureAddressMode::TEXTUREADDRESSMODE_WRAP;

	constBuffer = renderer->CreateConstBuffer(sizeof(localBuffer));
	
	localBuffer.intensity = 1;
	localBuffer.radius = 5;
	localBuffer.sigma = 5;
	localBuffer.threshold = 0.5;
}

void BloomRenderer::Init(LPCWSTR dirr) {}

void BloomRenderer::Render(GraphicsBase& gfx) {
	int32_t width, height;
	renderer->GetBackbufferSize(&width, &height);

	RenderTargetBinding* targets[3];

	gfx.texturesManger.CreateRenderTarget(SURFACEFORMAT_COLOR, width, height, bloom1, bloom1RT);
	gfx.texturesManger.CreateRenderTarget(SURFACEFORMAT_COLOR, width, height, bloom2, bloom2RT);

	renderer->ApplyVertexBufferBinding(vertexBuffer);
	renderer->ApplyIndexBufferBinding(indexBuffer, 16);

	RenderIMGUI(gfx);
	
	renderer->VerifyConstBuffer(constBuffer, BloomCosntants.slot);
	renderer->SetConstBuffer(constBuffer, &localBuffer);
	
	targets[0] = &bloom1RT;
	renderer->ApplyPipelineState(factory->GetState(BLOOMTHRESHOLD));
	renderer->SetRenderTargets(targets, 1, nullptr, DepthFormat::DEPTHFORMAT_D32, Viewport());
	renderer->VerifyPixelSampler(0, sampler);
	renderer->VerifyPixelTexture(0, gfx.texturesManger.luminance);
	renderer->VerifyPixelTexture(1, gfx.texturesManger.diffuseColor);
	renderer->DrawIndexedPrimitives(PrimitiveType::PRIMITIVETYPE_TRIANGLESTRIP, 0, 0, 0, 0, 2);

	targets[0] = &bloom2RT;
	targets[1] = &bloom1RT;
	for (int i = 0; i < 4; i++)
	{
		renderer->SetRenderTargets(targets, 1, nullptr, DepthFormat::DEPTHFORMAT_D32, Viewport());
		renderer->VerifyPixelTexture(0, targets[1]->texture);
		
		renderer->ApplyPipelineState(factory->GetState(BLOOMVERTICAL));
		renderer->DrawIndexedPrimitives(PrimitiveType::PRIMITIVETYPE_TRIANGLESTRIP, 0, 0, 0, 0, 2);
		targets[2] = targets[0];
		targets[0] = targets[1];
		targets[1] = targets[2];

		renderer->SetRenderTargets(targets, 1, nullptr, DepthFormat::DEPTHFORMAT_D32, Viewport());
		renderer->VerifyPixelTexture(0, targets[1]->texture);

		renderer->ApplyPipelineState(factory->GetState(BLOOMHORISONTAL));
		renderer->DrawIndexedPrimitives(PrimitiveType::PRIMITIVETYPE_TRIANGLESTRIP, 0, 0, 0, 0, 2);
		
		targets[2] = targets[0];
		targets[0] = targets[1];
		targets[1] = targets[2];
	}

	targets[0] = &gfx.texturesManger.bloomBluredRT;
	
	renderer->SetRenderTargets(targets, 1, nullptr, DepthFormat::DEPTHFORMAT_D32, Viewport());
	renderer->ApplyPipelineState(factory->GetState(BLOOMEND));
	renderer->VerifyPixelTexture(0, targets[1]->texture);

	renderer->DrawIndexedPrimitives(PrimitiveType::PRIMITIVETYPE_TRIANGLESTRIP, 0, 0, 0, 0, 2);
	
}

BloomRenderer::~BloomRenderer(){
	delete vertexBuffer.vertexOffset;
	delete vertexBuffer.vertexStride;
	renderer->AddDisposeVertexBuffer(vertexBuffer.vertexBuffers[0]);
	delete[] vertexBuffer.vertexBuffers;
	renderer->AddDisposeIndexBuffer(indexBuffer);
	renderer->AddDisposeConstBuffer(constBuffer);
	//delete provider;
	delete factory;
}

void BloomRenderer::RenderIMGUI(GraphicsBase& gfx)
{	
	ImGui::Begin("Bloom settings.");                          // Create a window called "Hello, world!" and append into it.
          
	ImGui::SliderFloat("Intensity.", &localBuffer.intensity, 0.0f, 1.0f);
	
	ImGui::SliderInt("Radius.", &localBuffer.radius, 0, 10);
	       
	ImGui::SliderFloat("Sigma.", &localBuffer.sigma, 0.01f, 5.0f);
	
	ImGui::SliderFloat("Threshold.", &localBuffer.threshold, 0.0f, 1.0);
	
	ImGui::End();
}
