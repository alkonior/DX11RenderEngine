#include "UIRenderer.h"

using namespace Renderer;
using namespace DirectX::SimpleMath;;

UIRendererProvider::UIRendererProvider(int32_t width, int32_t height):width(width), height(height){}

void UIRendererProvider::PatchPipelineState(PipelineState* refToPS) {

	//refToPS->bs.alphaBlendFunction = ;

	//state_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	//state_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	refToPS->bs.colorBlendFunction = BLENDFUNCTION_ADD;;
	refToPS->bs.alphaBlendFunction = BLENDFUNCTION_ADD;

	//state_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	//state_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	refToPS->bs.colorSourceBlend = Blend::BLEND_SOURCEALPHA;
	refToPS->bs.colorDestinationBlend = Blend::BLEND_INVERSESOURCEALPHA;


	//state_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	//state_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	refToPS->bs.alphaSourceBlend = Blend::BLEND_ONE;
	refToPS->bs.alphaDestinationBlend = Blend::BLEND_ZERO;

	//state_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	refToPS->bs.colorWriteEnable = ColorWriteChannels::COLORWRITECHANNELS_ALL;
	refToPS->bs.colorWriteEnable1 = ColorWriteChannels::COLORWRITECHANNELS_ALL;
	refToPS->bs.colorWriteEnable2 = ColorWriteChannels::COLORWRITECHANNELS_ALL;
	refToPS->bs.colorWriteEnable3 = ColorWriteChannels::COLORWRITECHANNELS_ALL;


	refToPS->bs.blendFactor = Renderer::Color{ 255,255,255,255 };

	refToPS->bf = Renderer::Color{ 255,255,255,255 };



	refToPS->rs.cullMode = CullMode::CULLMODE_NONE;
	refToPS->rs.depthBias = 0.0f;
	refToPS->rs.fillMode = FillMode::FILLMODE_SOLID;
	refToPS->rs.multiSampleAntiAlias = 0;
	refToPS->rs.scissorTestEnable = 0;
	refToPS->rs.slopeScaleDepthBias = 0.0f;


	refToPS->vp.x = 0;
	refToPS->vp.y = 0;
	refToPS->vp.w = width;
	refToPS->vp.h = height;
	refToPS->vp.minDepth = 0.0f;
	refToPS->vp.maxDepth = 1.0f;
}

UIRenderer::UIRenderer(Renderer::IRenderer* renderer) :renderer(renderer) {}

void UIRenderer::Init(void* shaderData, size_t dataSize) {

	int32_t width, height;
	renderer->GetBackbufferSize(&width, &height);
	factory = new UIRendererFactory(renderer, new UIRendererProvider(width, height), shaderData, dataSize);

	HRESULT hr;
	Vertex2D vertices[ ] =
	{
		{  Vector2(-1.0f, 1.0f),  Vector2(0.0f, 1.0f), },
		{  Vector2(-1.0f, 0.0f),  Vector2(0.0f, 0.0f), },
		{  Vector2(0.0f, 1.0f),  Vector2(1.0f, 1.0f), },
		{  Vector2(0.0f, 0.0f),  Vector2(1.0f, 0.0f), }
	};
	vertexBuffer.vertexBuffer = renderer->GenVertexBuffer(0, BufferUsage::BUFFERUSAGE_NONE, sizeof(vertices));
	renderer->SetVertexBufferData(vertexBuffer.vertexBuffer, 0, &vertices, 4, sizeof(Vertex2D), sizeof(Vertex2D), SetDataOptions::SETDATAOPTIONS_NONE);
	//GFX_THROW_INFO(gfx.pDevice->CreateBuffer(&bd, &sd, &pVertexBuffer));
	vertexBuffer.vertexOffset = 0;
	vertexBuffer.vertexStride = sizeof(Vertex2D);
	// Bind vertex buffer to pipeline


	// create index buffer
	const uint16_t indices[ ] =
	{
		0,1,2,
		1,3,2
	};
	indexBuffer = renderer->GenIndexBuffer(0, BufferUsage::BUFFERUSAGE_WRITEONLY, 12);
	renderer->SetIndexBufferData(indexBuffer, 0, (void*)indices, 12, SetDataOptions::SETDATAOPTIONS_DISCARD);



	constBuffer = renderer->CreateConstBuffer(sizeof(localBuffer));


	sampler.filter = TextureFilter::TEXTUREFILTER_POINT;
	sampler.addressU = TextureAddressMode::TEXTUREADDRESSMODE_WRAP;
	sampler.addressV = TextureAddressMode::TEXTUREADDRESSMODE_WRAP;
	sampler.addressW = TextureAddressMode::TEXTUREADDRESSMODE_WRAP;

}

void UIRenderer::Init(LPCWSTR dirr) {
	wrl::ComPtr<ID3DBlob> buff;
	D3DReadFileToBlob((std::wstring(dirr) + L"\\Shader2D.hlsl").c_str(), &buff);
	auto data = buff->GetBufferPointer();
	auto size = buff->GetBufferSize();
	Init(data, size);
}

void UIRenderer::Render() {
	int32_t width, height;
	renderer->GetBackbufferSize(&width, &height);
	uint32_t lastFlag = -1;

	renderer->ApplyVertexBufferBinding(&vertexBuffer);
	renderer->VerifyConstBuffers(&constBuffer, 1);
	renderer->SetRenderTargets(NULL, 0, NULL, DepthFormat::DEPTHFORMAT_NONE, 0);


	for (size_t i = 0; i < drawCalls.size(); i++) {
		if (drawCalls[i].flag != lastFlag) {
			renderer->ApplyPipelineState(factory->GetState(drawCalls[i].flag));
		}
		lastFlag = drawCalls[i].flag;

		auto  pTexture = drawCalls[i].texture.texture;
		renderer->VerifyPixelSampler(0, pTexture, sampler);
		localBuffer.transform = drawCalls[i].getTransform(width, height).Transpose();
		localBuffer.uvShift = drawCalls[i].getUVShift();
		localBuffer.uvScale = drawCalls[i].getUVScale();
		renderer->SetConstBuffer(constBuffer, &localBuffer);

		renderer->DrawIndexedPrimitives(PrimitiveType::PRIMITIVETYPE_TRIANGLELIST,
			0, 0, 0, 0, 2, indexBuffer, 16);
	}
	drawCalls.clear();
}

void UIRenderer::Draw(TexturesManager::TextureCache texture, size_t top, size_t left, size_t texW, size_t texH, size_t x, size_t y, size_t width, size_t height, uint32_t flag) {
	drawCalls.emplace_back(texture, top, left, texH, texW, x, y, width, height, flag);
}

void UIRenderer::Draw(TexturesManager::TextureCache texture, size_t x, size_t y, size_t width, size_t height, uint32_t flag) {
	drawCalls.emplace_back(texture, x, y, width, height, flag);
}



UIRenderer::Rectangle::Rectangle(TexturesManager::TextureCache texture, size_t top, size_t left, size_t texW, size_t texH, size_t x, size_t y, size_t width, size_t height, uint32_t flag)
	:x(x), y(y), width(width), height(height), texture(texture), top(top), left(left), texH(texH), texW(texW), flag(flag){}

UIRenderer::Rectangle::Rectangle(TexturesManager::TextureCache texture, size_t x, size_t y, size_t width, size_t height, uint32_t flag)
	: x(x), y(y), width(width), height(height), texture(texture), top(0), left(0), texH(texture.height), texW(texture.width), flag(flag) {}

dx::SimpleMath::Matrix UIRenderer::Rectangle::getTransform(size_t screenW, size_t screenH) {
	return

		Matrix::CreateScale(width * 1.0f, height * 1.0f, 0) *
		Matrix::CreateTranslation(x * 1.0f, y * 1.0f, 0) *
		Matrix::CreateScale(2.0f / screenW, 2.0f / screenH, 0) *
		Matrix::CreateTranslation(-1, -1, 0) *
		Matrix::CreateScale(1, -1, 0) *
		Matrix::CreateTranslation(width * 2.0f / screenW, 0, 0);
}

dx::SimpleMath::Vector2 UIRenderer::Rectangle::getUVShift() {
	return dx::SimpleMath::Vector2(top * 1.0f / texture.width, left * 1.0f / texture.height);
}

dx::SimpleMath::Vector2 UIRenderer::Rectangle::getUVScale() {
	return dx::SimpleMath::Vector2(texW * 1.0f / texture.width, texH * 1.0f / texture.height);
}