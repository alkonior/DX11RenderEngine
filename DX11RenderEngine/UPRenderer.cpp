#include "UPRenderer.h"


using namespace Renderer;


UPRenderer::UPRenderer(Renderer::IRenderer* renderer) : renderer(renderer) {}

void UPRenderer::Init(void* shaderData, size_t dataSize) {
	if (provider != nullptr) {
		//delete provider;
		delete factory;
		int32_t width, height;
		renderer->GetBackbufferSize(&width, &height);
		provider = new UPRendererProvider(width, height);
		factory = new UPRendererFactory(renderer, provider, shaderData, dataSize);
		return;
	}

	int32_t width, height;
	renderer->GetBackbufferSize(&width, &height);
	provider = new UPRendererProvider(width, height);
	factory = new UPRendererFactory(renderer, provider, shaderData, dataSize);


	vertexBuffer.buffersCount = 1;
	vertexBuffer.vertexBuffers = new Buffer * [1]();

	pTransformCB = renderer->CreateConstBuffer(sizeof(transformBuffer));
	pDataCB = renderer->CreateConstBuffer(sizeof(dataBuffer));
	vertexBuffer.vertexOffset = new (UINT)(0);
	vertexBuffer.vertexStride = new (UINT)(sizeof(UPVertex));


	sampler.filter = TextureFilter::TEXTUREFILTER_POINT;
	sampler.addressU = TextureAddressMode::TEXTUREADDRESSMODE_WRAP;
	sampler.addressV = TextureAddressMode::TEXTUREADDRESSMODE_WRAP;
	sampler.addressW = TextureAddressMode::TEXTUREADDRESSMODE_WRAP;
	sampler.mipMapLevelOfDetailBias = 0;
}

void UPRenderer::Init(LPCWSTR dirr) {
	wrl::ComPtr<ID3DBlob> buff;
	D3DReadFileToBlob((std::wstring(dirr) + L"\\BSPShader.hlsl").c_str(), &buff);
	auto data = buff->GetBufferPointer();
	auto size = buff->GetBufferSize();
	Init(data, size);
}

void UPRenderer::Draw(UPModelData model, TexturesManager::TextureCache texture, Transform position, size_t flags) {
	drawCalls.emplace_back(model, texture, position, flags);
}

void UPRenderer::Render(const GraphicsBase& gfx) {

	int32_t width, height;
	renderer->GetBackbufferSize(&width, &height);
	size_t lastFlags = -1;

	renderer->VerifyConstBuffers(&pTransformCB, 1);
	renderer->SetRenderTargets(NULL, 0, NULL, DepthFormat::DEPTHFORMAT_NONE, 0);

	transformBuffer.view = gfx.camera;
	transformBuffer.projection = gfx.cameraProjection;

	for (size_t i = 0; i < drawCalls.size(); i++) {
		if (drawCalls[i].flags != lastFlags) {
			renderer->ApplyPipelineState(factory->GetState(drawCalls[i].flags));
		}
		lastFlags = drawCalls[i].flags;

		ResizeBuffers(drawCalls[i].model.verticies.size(), drawCalls[i].model.indexes.size());
		renderer->SetVertexBufferData(vertexBuffer.vertexBuffers[0], 0, drawCalls[i].model.verticies.data(),
			drawCalls[i].model.verticies.size(), sizeof(UPVertex), sizeof(UPVertex), Renderer::SetDataOptions::SETDATAOPTIONS_DISCARD);
		renderer->SetIndexBufferData(indexBuffer, 0, drawCalls[i].model.indexes.data(), drawCalls[i].model.indexes.size() * 2, Renderer::SetDataOptions::SETDATAOPTIONS_DISCARD);

		renderer->ApplyVertexBufferBinding(&vertexBuffer);

		auto  pTexture = drawCalls[i].texture.texture;
		renderer->VerifyPixelSampler(0, pTexture, sampler);

		transformBuffer.world = drawCalls[i].position.GetTransform();

		//localBuffer.transform = drawCalls[i].getTransform(width, height).Transpose();
		//localBuffer.uvShift = drawCalls[i].getUVShift();
		//localBuffer.uvScale = drawCalls[i].getUVScale();
		renderer->SetConstBuffer(pTransformCB, &transformBuffer);
		renderer->DrawIndexedPrimitives(
			drawCalls[i].model.pt, 0, 0, 0, 0,
			drawCalls[i].model.primitiveCount, indexBuffer, 16);
	}


}

void UPRenderer::Clear() {
	drawCalls.clear();
}

void UPRenderer::Destroy() {
	delete vertexBuffer.vertexOffset;
	delete vertexBuffer.vertexStride;
	renderer->AddDisposeVertexBuffer(vertexBuffer.vertexBuffers[0]);
	delete[] vertexBuffer.vertexBuffers;
	renderer->AddDisposeIndexBuffer(indexBuffer);
	renderer->AddDisposeConstBuffer(pTransformCB);
	renderer->AddDisposeConstBuffer(pDataCB);
	delete provider;
	delete factory;
}

void UPRenderer::ResizeBuffers(size_t newVertexBuffSize, size_t newIndexBuffSize) {

	if (newVertexBuffSize > vertexBuffSize) {
		if (vertexBuffer.vertexBuffers[0] != nullptr)
			renderer->AddDisposeVertexBuffer(vertexBuffer.vertexBuffers[0]);
		vertexBuffer.vertexBuffers[0] = renderer->GenVertexBuffer(0, BufferUsage::BUFFERUSAGE_WRITEONLY, sizeof(UPVertex) * newVertexBuffSize);
		vertexBuffSize = newVertexBuffSize;
	}
	//GFX_THROW_INFO(gfx.pDevice->CreateBuffer(&bd, &sd, &pVertexBuffer));
	// Bind vertex buffer to pipeline

	if (newIndexBuffSize > indexBuffSize) {
		if (indexBuffer != nullptr)
			renderer->AddDisposeIndexBuffer(indexBuffer);
		indexBuffer = renderer->GenIndexBuffer(0, BufferUsage::BUFFERUSAGE_WRITEONLY, sizeof(uint16_t) * newIndexBuffSize);
		indexBuffSize = newIndexBuffSize;
	}
	//renderer->SetIndexBufferData(indexBuffer, 0, (void*)indices, 12, SetDataOptions::SETDATAOPTIONS_DISCARD);

}


UPRenderer::~UPRenderer() { Destroy(); }

UPRenderer::DrawCall::DrawCall(UPModelData model, TexturesManager::TextureCache texture, Transform position, uint64_t flags) :
	model(model), texture(texture), position(position), flags(flags) {}
