#include "BloomRenderer.h"

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

//const D3D11_INPUT_ELEMENT_DESC DefaultInputElements[] =
//{
//};


Renderer::InputLayoutDescription BloomRenderer::BloomRendererProvider::GetInputLayoutDescription(size_t definesFlags) {
	return Renderer::InputLayoutDescription{ (void*)nullptr, 0 };
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

	int32_t width, height;
	renderer->GetBackbufferSize(&width, &height);
	provider = new BloomRendererProvider(width, height);
	factory = new BloomRendererFactory(renderer, provider, shaderData, dataSize);


	//VertexBloom vertices[] =
	//{
	//	{  float2(-1.0f, 1.0f),  float2(0.0f, 0.0f), },
	//	{  float2(-1.0f, -1.0f),  float2(0.0f, 1.0f), },
	//	{  float2(1.0f,  1.0f),  float2(1.0f, 0.0f), },
	//	{  float2(1.0f,  -1.0f),  float2(1.0f, 1.0f), }
	//};
	//vertexBuffer.buffersCount = 1;
	//vertexBuffer.vertexBuffers = new Buffer * [1]();
	//vertexBuffer.vertexBuffers[0] = renderer->GenVertexBuffer(0, BufferUsage::BUFFERUSAGE_NONE, sizeof(vertices));
	//renderer->SetVertexBufferData(vertexBuffer.vertexBuffers[0], 0, &vertices, 4, sizeof(VertexBloom), sizeof(VertexBloom), SetDataOptions::SETDATAOPTIONS_NONE);
	////GFX_THROW_INFO(gfx.pDevice->CreateBuffer(&bd, &sd, &pVertexBuffer));
	//vertexBuffer.vertexOffset = new (UINT)(0);
	//vertexBuffer.vertexStride = new (UINT)(sizeof(VertexBloom));
	//// Bind vertex buffer to pipeline
	//
	//
	//// create index buffer
	//const uint16_t indices[] =
	//{
	//	0,1,2,3
	//};
	//indexBuffer = renderer->GenIndexBuffer(0, BufferUsage::BUFFERUSAGE_WRITEONLY, 12);
	//renderer->SetIndexBufferData(indexBuffer, 0, (void*)indices, 12, SetDataOptions::SETDATAOPTIONS_DISCARD);


	sampler.filter = TextureFilter::TEXTUREFILTER_POINT;
	sampler.addressU = TextureAddressMode::TEXTUREADDRESSMODE_WRAP;
	sampler.addressV = TextureAddressMode::TEXTUREADDRESSMODE_WRAP;
	sampler.addressW = TextureAddressMode::TEXTUREADDRESSMODE_WRAP;
}

void BloomRenderer::Init(LPCWSTR dirr) {}

void BloomRenderer::Render(GraphicsBase& gfx) {

	RenderTargetBinding* targets[4];// = {
	// &gfx.texturesManger.diffuseColorRT,
	// &gfx.texturesManger.directLightsRT,
	// &gfx.texturesManger.blumeMaskRT,
	// &gfx.texturesManger.alphaSurfacesRT,
	//};

	targets[0] = &gfx.texturesManger.bloomBluredRT;
	targets[1] = &gfx.texturesManger.blumeMaskRT;


	renderer->ApplyPipelineState(factory->GetState(BLOOMZERO));
	renderer->VerifyPixelSampler(0, sampler);

		renderer->SetRenderTargets(targets, 1, nullptr, DepthFormat::DEPTHFORMAT_D32, Viewport());
		renderer->VerifyPixelTexture(0, targets[1]->texture);
		renderer->DrawIndexedPrimitives(PrimitiveType::PRIMITIVETYPE_TRIANGLESTRIP, 0, 0, 0, 0, 2);
		auto  buff = targets[0];
		targets[0] = targets[1];
		targets[1] = buff;
	


	//renderer->ApplyPipelineState(factory->GetState(ENDZERO));
	//renderer->SetRenderTargets(nullptr, 0, nullptr, DepthFormat::DEPTHFORMAT_D32, vp);
	//renderer->VerifyPixelSampler(0, sampler);
	//
	//renderer->VerifyPixelTexture(0, gfx.texturesManger.diffuseColor);
	//renderer->VerifyPixelTexture(1, gfx.texturesManger.directLights);
	//renderer->VerifyPixelTexture(2, targets[0]->texture);
	//
	//renderer->DrawIndexedPrimitives(PrimitiveType::PRIMITIVETYPE_TRIANGLESTRIP, 0, 0, 0, 0, 2);
	//renderer->ApplyPipelineState(factory->GetState(ENDALPHA));
	//renderer->VerifyPixelTexture(3, gfx.texturesManger.alphaSurfaces);
	//renderer->DrawIndexedPrimitives(PrimitiveType::PRIMITIVETYPE_TRIANGLESTRIP, 0, 0, 0, 0, 2);


}

BloomRenderer::~BloomRenderer() {}
