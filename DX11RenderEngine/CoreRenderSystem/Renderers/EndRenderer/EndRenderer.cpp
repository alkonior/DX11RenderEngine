#include "EndRenderer.h"

#include "imgui/imgui.h"

using namespace Renderer;

EndRenderer::EndRendererProvider::EndRendererProvider(int32_t width, int32_t height) {}

void EndRenderer::EndRendererProvider::PatchPipelineState(Renderer::PipelineState* refToPS, size_t definesFlags) {

	refToPS->bs = &BlendStates::NoAlpha;
	
	if (definesFlags & (ENDALPHA)) {
		refToPS->bs = &BlendStates::Alpha;
	}

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


Renderer::InputLayoutDescription EndRenderer::EndRendererProvider::GetInputLayoutDescription(size_t definesFlags) {
	return Renderer::InputLayoutDescription{ (void*)DefaultInputElements, std::size(DefaultInputElements) };
}

EndRenderer::EndRendererProvider::~EndRendererProvider() {}

EndRenderer::EndRenderer():renderer(IRenderer::renderer) {}

void EndRenderer::Init(void* shaderData, size_t dataSize) {
	if (provider != nullptr) {
		//delete provider;
		delete factory;
		int32_t width, height;
		renderer->GetBackbufferSize(&width, &height);
		provider = new EndRendererProvider(width, height);
		factory = new EndRendererFactory(renderer, provider, shaderData, dataSize);
		return;
	}

	int32_t width, height;
	renderer->GetBackbufferSize(&width, &height);
	provider = new EndRendererProvider(width, height);
	factory = new EndRendererFactory(renderer, provider, shaderData, dataSize);


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
}

void EndRenderer::Init(LPCWSTR dirr) {}

void EndRenderer::Render(GraphicsBase& gfx) {

	RenderIMGUI(gfx);
	
	uint64_t flags = 0;
	if (lightOnly)
		flags|=ENDLIGHTONLY;
	if (colorOnly)
		flags|=ENDCOLORONLY;
	if (bloomOnly)
		flags|=ENDBLOOMONLY;
	if (alphaOnly)
		flags|=ENDALPHAONLY;

	
	renderer->ApplyVertexBufferBinding(vertexBuffer);
	renderer->ApplyIndexBufferBinding(indexBuffer, 16);

	RenderTargetBinding* targets[1] = {&gfx.texturesManger.preFXAAcolorRT};
	renderer->SetRenderTargets(targets, 1, nullptr, vp);
	
	renderer->VerifyPixelSampler(0, Samplers::point);

	renderer->VerifyPixelTexture(0, gfx.texturesManger.diffuseColor);
	renderer->VerifyPixelTexture(2, gfx.texturesManger.directLights);
	renderer->VerifyPixelTexture(1, gfx.texturesManger.bloomBlured);

	renderer->ApplyPipelineState(factory->GetState(ENDZERO | flags));
	renderer->DrawIndexedPrimitives(PrimitiveType::PRIMITIVETYPE_TRIANGLESTRIP, 0, 0, 0, 0, 2);
	renderer->ApplyPipelineState(factory->GetState(ENDALPHA | flags));
	renderer->VerifyPixelTexture(3, gfx.texturesManger.alphaSurfaces);
	renderer->DrawIndexedPrimitives(PrimitiveType::PRIMITIVETYPE_TRIANGLESTRIP, 0, 0, 0, 0, 2);


}


void EndRenderer::RenderIMGUI(GraphicsBase& gfx)
{	
	ImGui::Begin("Postprocess settings.");                          // Create a window called "Hello, world!" and append into it.

	ImGui::Checkbox("ColorOnly", &colorOnly);
	ImGui::Checkbox("LightOnly", &lightOnly);
	ImGui::Checkbox("BloomOnly", &bloomOnly);
	ImGui::Checkbox("AlphaOnly", &alphaOnly);
	
	ImGui::End();
}


EndRenderer::~EndRenderer()
{
	delete vertexBuffer.vertexOffset;
	delete vertexBuffer.vertexStride;
	renderer->AddDisposeVertexBuffer(vertexBuffer.vertexBuffers[0]);
	delete[] vertexBuffer.vertexBuffers;
	renderer->AddDisposeIndexBuffer(indexBuffer);
	//renderer->AddDisposeConstBuffer(constBuffer);
	//delete provider;
	delete factory;
}
