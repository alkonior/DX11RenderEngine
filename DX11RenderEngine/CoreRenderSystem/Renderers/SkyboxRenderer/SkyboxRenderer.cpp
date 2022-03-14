#include "SkyboxRenderer.h"


using namespace Renderer;
using namespace DirectX::SimpleMath;;


SkyboxRenderer::SkyboxRenderer() :renderer(IRenderer::renderer) {}

void SkyboxRenderer::Init(void* shaderData, size_t dataSize) {
	if (provider != nullptr) {
		//delete provider;
		delete factory;
		int32_t width, height;
		renderer->GetBackbufferSize(&width, &height);
		provider = new SkyboxRendererProvider(width, height);
		factory = new SkyboxRendererFactory(renderer, provider, shaderData, dataSize);
		return;
	}

	int32_t width, height;
	renderer->GetBackbufferSize(&width, &height);
	provider = new SkyboxRendererProvider(width, height);
	factory = new SkyboxRendererFactory(renderer, provider, shaderData, dataSize);

	constexpr float side = 1.0f;
	VertexSkybox vertices[8] =
	{
		VertexSkybox(float3( -side,-side,-side )),
		VertexSkybox(float3( side,-side,-side )),
		VertexSkybox(float3( -side,side,-side )),
		VertexSkybox(float3( side,side,-side )),
		VertexSkybox(float3( -side,-side,side )),
		VertexSkybox(float3( side,-side,side )),
		VertexSkybox(float3( -side,side,side )),
		VertexSkybox(float3( side,side,side )),
	};


	vertexBuffer.buffersCount = 1;
	vertexBuffer.vertexBuffers = new Buffer * [1]();
	vertexBuffer.vertexBuffers[0] = renderer->GenVertexBuffer(0, BufferUsage::BUFFERUSAGE_NONE, sizeof(vertices));
	renderer->SetVertexBufferData(vertexBuffer.vertexBuffers[0], 0, &vertices, std::size(vertices), sizeof(VertexSkybox), sizeof(VertexSkybox), SetDataOptions::SETDATAOPTIONS_NONE);
	//GFX_THROW_INFO(gfx.pDevice->CreateBuffer(&bd, &sd, &pVertexBuffer));
	vertexBuffer.vertexOffset = new (UINT)(0);
	vertexBuffer.vertexStride = new (UINT)(sizeof(VertexSkybox));
	// Bind vertex buffer to pipeline


	// create index buffer
	const uint16_t indices[] =
	{
			0,2,1, 2,3,1,
			1,3,5, 3,7,5,
			2,6,3, 3,6,7,
			4,5,7, 4,7,6,
			0,4,2, 2,4,6,
			0,1,4, 1,5,4
	};
	indexBuffer = renderer->GenIndexBuffer(0, BufferUsage::BUFFERUSAGE_WRITEONLY, std::size(indices) * 2);
	renderer->SetIndexBufferData(indexBuffer, 0, (void*)indices, std::size(indices)*2, SetDataOptions::SETDATAOPTIONS_DISCARD);


	skyTexture = renderer->CreateTextureCube(SurfaceFormat::SURFACEFORMAT_COLOR, 256, 1, false);

	constBuffer = renderer->CreateConstBuffer(sizeof(localBuffer));





	vp.x = 0;
	vp.y = 0;
	vp.w = width;
	vp.h = height;
	vp.minDepth = 0.0f;
	vp.maxDepth = 1.0f;

}

void SkyboxRenderer::Init(LPCWSTR dirr) {
	wrl::ComPtr<ID3DBlob> buff;
	D3DReadFileToBlob((std::wstring(dirr) + L"\\Shader2D.hlsl").c_str(), &buff);
	auto data = buff->GetBufferPointer();
	auto size = buff->GetBufferSize();
	Init(data, size);
}

//int sideSequence[6] = { 3, 1, 4, 5, 0, 2 };
//
//int sideSequence[6] = ;

void SkyboxRenderer::UpdateSkybox(const TextureData& sky, int side) {
	renderer->SetTextureDataCube(skyTexture, 0, 0, sky.GetWidth(), sky.GetHeight(), side, 0, (void*)sky.GetBufferPtr(), 0);
}

void SkyboxRenderer::Destroy() {

	delete vertexBuffer.vertexOffset;
	delete vertexBuffer.vertexStride;
	renderer->AddDisposeVertexBuffer(vertexBuffer.vertexBuffers[0]);
	delete[] vertexBuffer.vertexBuffers;
	renderer->AddDisposeIndexBuffer(indexBuffer);
	renderer->AddDisposeConstBuffer(constBuffer);
	//delete provider;
	delete factory;
}

void SkyboxRenderer::Render(GraphicsBase& gfx) {
	if (gfx.skyFlags & SKYNOTHING) return;
	int32_t width, height;
	renderer->GetBackbufferSize(&width, &height);
	renderer->ApplyPipelineState(factory->GetState(gfx.skyFlags));
	renderer->ApplyVertexBufferBinding(vertexBuffer);
	renderer->ApplyIndexBufferBinding(indexBuffer, 16);

	RenderTargetBinding* targets[] = { (RenderTargetBinding*)&gfx.texturesManger.preFXAAcolorRT };
	renderer->SetRenderTargets(targets, 1, gfx.texturesManger.depthRBuffer, vp);

	renderer->VerifyConstBuffer(constBuffer, skyboxCosntBuffer.slot);
	localBuffer.skyboxView = (matrix::CreateRotationX(1.570796)*( gfx.localConstants.view.Transpose())).Transpose();
	renderer->SetConstBuffer(constBuffer, &localBuffer);

	renderer->VerifyPixelSampler(0, Samplers::anisotropic16);
	renderer->VerifyPixelTexture(0, skyTexture);

	renderer->DrawIndexedPrimitives(PrimitiveType::PRIMITIVETYPE_TRIANGLELIST,
		0, 0, 0, 0, 12);

}

SkyboxRenderer::~SkyboxRenderer() { Destroy(); }


SkyboxRenderer::SkyboxRendererProvider::SkyboxRendererProvider(int32_t width, int32_t height) :width(width), height(height) {}

void SkyboxRenderer::SkyboxRendererProvider::PatchPipelineState(PipelineState* refToPS, size_t definesFlags) {

	//refToPS->bs.alphaBlendFunction = ;


	refToPS->bs = &BlendStates::NoAlpha;



	refToPS->dss.depthBufferEnable = true;
	refToPS->dss.depthBufferWriteEnable = false;
	refToPS->dss.depthBufferFunction = CompareFunction::COMPAREFUNCTION_LESSEQUAL;
	refToPS->dss.stencilEnable = false;


	refToPS->rs.cullMode = CullMode::CULLMODE_NONE;
	refToPS->rs.depthBias = 0.0f;
	refToPS->rs.fillMode = FillMode::FILLMODE_SOLID;
	refToPS->rs.multiSampleAntiAlias = 0;
	refToPS->rs.scissorTestEnable = 0;
	refToPS->rs.slopeScaleDepthBias = 0.0f;
}


const D3D11_INPUT_ELEMENT_DESC SkyboxInputElements[] =
{
		{"Position",  0, DXGI_FORMAT_R32G32B32_FLOAT,  0,                            0, D3D11_INPUT_PER_VERTEX_DATA, 0},
};

InputLayoutDescription SkyboxRenderer::SkyboxRendererProvider::GetInputLayoutDescription(size_t definesFlags) {
	return InputLayoutDescription{ (void*)SkyboxInputElements, std::size(SkyboxInputElements) };
}

SkyboxRenderer::SkyboxRendererProvider::~SkyboxRendererProvider() {}

