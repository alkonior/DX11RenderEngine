#include "UIRenderer.h"

using namespace Renderer;
using namespace DirectX::SimpleMath;;


UIRenderer::UIRenderer() : BaseRenderer("UIShader.hlsl") {}

void UIRenderer::Init(void* shaderData, size_t dataSize) {
	if (provider != nullptr) {
		//delete provider;
		delete factory;
		int32_t width, height;
		renderer->GetBackbufferSize(&width, &height);
		provider = new UIRendererProvider(width, height);
		factory = new UIRendererFactory(provider, shaderData, dataSize);
		return;
	}

	int32_t width, height;
	renderer->GetBackbufferSize(&width, &height);
	provider = new UIRendererProvider(width, height);
	factory = new UIRendererFactory(provider, shaderData, dataSize);

	Vertex2D vertices[] =
	{
		{  float2(-1.0f, 1.0f),  float2(0.0f, 1.0f), },
		{  float2(-1.0f, 0.0f),  float2(0.0f, 0.0f), },
		{  float2(0.0f,  1.0f),  float2(1.0f, 1.0f), },
		{  float2(0.0f,  0.0f),  float2(1.0f, 0.0f), }
	};
	vertexBuffer.buffersCount = 1;
	vertexBuffer.vertexBuffers = new Buffer * [1]();
	vertexBuffer.vertexBuffers[0] = renderer->GenVertexBuffer(0, BufferUsage::BUFFERUSAGE_NONE, sizeof(vertices));
	renderer->SetVertexBufferData(vertexBuffer.vertexBuffers[0], 0, &vertices, 4, sizeof(Vertex2D), sizeof(Vertex2D), SetDataOptions::SETDATAOPTIONS_NONE);
	//GFX_THROW_INFO(gfx.pDevice->CreateBuffer(&bd, &sd, &pVertexBuffer));
	vertexBuffer.vertexOffset = new (UINT)(0);
	vertexBuffer.vertexStride = new (UINT)(sizeof(Vertex2D));
	// Bind vertex buffer to pipeline


	// create index buffer
	const uint16_t indices[] =
	{
		0,1,2,
		1,3,2
	};
	indexBuffer = renderer->GenIndexBuffer(0, BufferUsage::BUFFERUSAGE_WRITEONLY, 12);
	renderer->SetIndexBufferData(indexBuffer, 0, (void*)indices, 12, SetDataOptions::SETDATAOPTIONS_DISCARD);



	vp.x = 0;
	vp.y = 0;
	vp.w = width;
	vp.h = height;
	vp.minDepth = 0.0f;
	vp.maxDepth = 1.0f;

	constBuffer = renderer->CreateConstBuffer(sizeof(localBuffer));


	sampler.filter = TextureFilter::TEXTUREFILTER_POINT;
	sampler.addressU = TextureAddressMode::TEXTUREADDRESSMODE_WRAP;
	sampler.addressV = TextureAddressMode::TEXTUREADDRESSMODE_WRAP;
	sampler.addressW = TextureAddressMode::TEXTUREADDRESSMODE_WRAP;

}

void UIRenderer::Destroy() {

	delete vertexBuffer.vertexOffset;
	delete vertexBuffer.vertexStride;
	renderer->AddDisposeVertexBuffer(vertexBuffer.vertexBuffers[0]);
	delete[] vertexBuffer.vertexBuffers;
	renderer->AddDisposeIndexBuffer(indexBuffer);
	renderer->AddDisposeConstBuffer(constBuffer);
	//delete provider;
	delete factory;
}

void UIRenderer::Render() {
	int32_t width, height;
	renderer->GetBackbufferSize(&width, &height);
	uint64_t lastFlag = -1;
	//renderer->ApplyPipelineState(factory->GetState(0));

	renderer->ApplyVertexBufferBinding(vertexBuffer);
	renderer->ApplyIndexBufferBinding(indexBuffer, 16);
	renderer->VerifyPixelSampler(0, Samplers::pointClamp);

	renderer->SetRenderTargets(nullptr, 0, nullptr, vp);
	renderer->VerifyConstBuffer(constBuffer, UITransform.slot);

	for (size_t i = 0; i < drawCalls.size(); i++) {
		if (drawCalls[i].data.flag != lastFlag) {
			if (drawCalls[i].data.flag & UICHAR) { renderer->BeginEvent("Chars"); }
			if (lastFlag & UICHAR) { renderer->EndEvent(); }
			renderer->ApplyPipelineState(factory->GetState(drawCalls[i].data.flag));
			lastFlag = drawCalls[i].data.flag;
		}

		localBuffer.transform = drawCalls[i].getTransform(width, height).Transpose();
		localBuffer.uvShift = drawCalls[i].getUVShift();
		localBuffer.uvScale = drawCalls[i].getUVScale();
		localBuffer.color = drawCalls[i].data.color;

		renderer->SetConstBuffer(constBuffer, &localBuffer);

		auto  pTexture = drawCalls[i].texture.texture;
		renderer->VerifyPixelTexture(0, pTexture);


		renderer->DrawIndexedPrimitives(PrimitiveType::PRIMITIVETYPE_TRIANGLELIST,
			0, 0, 0, 0, 2);

	}
	if (lastFlag & UICHAR) { renderer->EndEvent(); }
}

void UIRenderer::Clear() {
	drawCalls.clear();
}

UIRenderer::~UIRenderer() { Destroy(); }

void UIRenderer::Draw(TexturesManager::TextureCache texture, const UIDrawData& data) {
	drawCalls.push_back(DrawCall(texture, data));
}


void UIRenderer::Draw(const UIDrawData& data) {
	drawCalls.push_back(DrawCall(data));
}

UIRenderer::DrawCall::DrawCall(TexturesManager::TextureCache texture, const UIDrawData& data) :data(data), texture(texture) {}

UIRenderer::DrawCall::DrawCall(const UIDrawData& data) : data(data) {}

matrix UIRenderer::DrawCall::getTransform(size_t screenW, size_t screenH) {
	return

		Matrix::CreateScale(data.width * 1.0f, data.height * 1.0f, 0) *
		Matrix::CreateTranslation(data.x * 1.0f, data.y * 1.0f, 0) *
		Matrix::CreateScale(2.0f / screenW, 2.0f / screenH, 0) *
		Matrix::CreateTranslation(-1, -1, 0) *
		Matrix::CreateScale(1, -1, 0) *
		Matrix::CreateTranslation(data.width * 2.0f / screenW, 0, 0);
}

float2 UIRenderer::DrawCall::getUVShift() {
	return float2(data.top * 1.0f / texture.width, data.left * 1.0f / texture.height);
}

float2 UIRenderer::DrawCall::getUVScale() {
	return float2(data.texW * 1.0f / texture.width, data.texH * 1.0f / texture.height);
}





UIRenderer::UIRendererProvider::UIRendererProvider(int32_t width, int32_t height) :width(width), height(height) {}

void UIRenderer::UIRendererProvider::PatchPipelineState(PipelineState* refToPS, size_t definesFlags) {

	refToPS->bs = &BlendStates::Alpha;

	refToPS->dss = &DepthStencilStates::NoDSS;
	refToPS->rs = &RasterizerStates::CClockWise;
	
}


const D3D11_INPUT_ELEMENT_DESC UIInputElements[] =
{
		{"Position",  0, DXGI_FORMAT_R32G32_FLOAT,  0,                            0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
};

InputLayoutDescription UIRenderer::UIRendererProvider::GetInputLayoutDescription(size_t definesFlags) {
	return InputLayoutDescription{ (void*)UIInputElements, std::size(UIInputElements) };
}

const char* UIRenderer::UIRendererProvider::GetShaderName()
{
	return "UI shader";
}


