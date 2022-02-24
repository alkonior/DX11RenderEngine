#include "UPRenderer.h"
#include "UPRenderer.h"


using namespace Renderer;


UPRenderer::UPRenderer(Renderer::IRenderer* renderer) :
	renderer(renderer), staticMeshes(renderer, 20000, 20000), dynamicMeshes(renderer, 2000, 2000) {}

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

	pTransformCB = renderer->CreateConstBuffer(sizeof(transformBuffer));
	pDataCB = renderer->CreateConstBuffer(sizeof(dataBuffer));


	sampler.filter = TextureFilter::TEXTUREFILTER_POINT;
	sampler.addressU = TextureAddressMode::TEXTUREADDRESSMODE_WRAP;
	sampler.addressV = TextureAddressMode::TEXTUREADDRESSMODE_WRAP;
	sampler.addressW = TextureAddressMode::TEXTUREADDRESSMODE_WRAP;
	sampler.mipMapLevelOfDetailBias = 0;

	lightSampler.filter = TextureFilter::TEXTUREFILTER_ANISOTROPIC;
	lightSampler.addressU = TextureAddressMode::TEXTUREADDRESSMODE_WRAP;
	lightSampler.addressV = TextureAddressMode::TEXTUREADDRESSMODE_WRAP;
	lightSampler.addressW = TextureAddressMode::TEXTUREADDRESSMODE_WRAP;
	lightSampler.mipMapLevelOfDetailBias = 0;
	lightSampler.maxAnisotropy = 16;
}

void UPRenderer::Init(LPCWSTR dirr) {
	wrl::ComPtr<ID3DBlob> buff;
	D3DReadFileToBlob((std::wstring(dirr) + L"\\BSPShader.hlsl").c_str(), &buff);
	auto data = buff->GetBufferPointer();
	auto size = buff->GetBufferSize();
	Init(data, size);
}

MeshHashData UPRenderer::Register(UPModelData model, bool dynamic) {
	if (dynamic) {
		return dynamicMeshes.AddMesh(model);
	}
	else {
		return staticMeshes.AddMesh(model);
	}
}

void UPRenderer::Draw(MeshHashData model, TexturesManager::TextureCache texture, UPDrawData data) {
	if (drawCalls.size()) {
		auto& lastDrawCall = drawCalls[drawCalls.size() - 1];
		if ((lastDrawCall.data == data) &&
			(lastDrawCall.model.indexOffset + lastDrawCall.model.numElem == model.indexOffset)
			&& texture.texture == lastDrawCall.texture.texture) {
			lastDrawCall.model.numElem += model.numElem;
			return;
		}
	}
	drawCalls.emplace_back(model, texture, data);
}
void UPRenderer::Draw(MeshHashData model, TexturesManager::TextureCache texture, TexturesManager::TextureCache lightMap, UPDrawData data) {
	if (drawCalls.size()) {
		auto& lastDrawCall = drawCalls[drawCalls.size() - 1];
		if ((lastDrawCall.data == data)
			&& (lastDrawCall.model.indexOffset + lastDrawCall.model.numElem == model.indexOffset)
			&& (texture.texture == lastDrawCall.texture.texture)
			&& (lightMap.texture == lastDrawCall.lightMap.texture)) {
			lastDrawCall.model.numElem += model.numElem;
			return;
		}
	}
	drawCalls.emplace_back(model, texture, lightMap, data);
}

void UPRenderer::DrawSet(MeshHashData model, UPModelData newModel, TexturesManager::TextureCache texture, UPDrawData data) {

	if (data.dynamic) {
		dynamicMeshes.UpdateMesh(model, newModel);
	}
	else {
		printf("Static Mesh Updated. Fix this !!!!!!!!!!!!\n");
		staticMeshes.UpdateMesh(model, newModel);
	}

	drawCalls.emplace_back(model, texture, data);
}

void UPRenderer::Render(const GraphicsBase& gfx) {
	

	staticMeshes.UpdateBuffers();
	dynamicMeshes.UpdateBuffers(true);



	int32_t width, height;
	renderer->GetBackbufferSize(&width, &height);
	size_t lastFlags = -1;

	renderer->VerifyConstBuffer(pTransformCB, UPTransform.slot);
	renderer->VerifyConstBuffer(pDataCB, UPExtraData.slot);
	renderer->SetRenderTargets(NULL, 0, NULL, DepthFormat::DEPTHFORMAT_NONE, 0);

	transformBuffer.view = gfx.viewMatrix;
	transformBuffer.projection = gfx.cameraProjection;
	renderer->SetConstBuffer(pTransformCB, &transformBuffer);

	for (size_t i = 0; i < drawCalls.size(); i++) {
		if (drawCalls[i].data.flags != lastFlags) {
			renderer->ApplyPipelineState(factory->GetState(drawCalls[i].data.flags));
		}
		lastFlags = drawCalls[i].data.flags;



		auto  pTexture = drawCalls[i].texture.texture;
		renderer->VerifyPixelSampler(0, pTexture, sampler);

		auto  pLightMap = drawCalls[i].lightMap.texture;
		if (drawCalls[i].data.flags & UPLIGHTMAPPED)
			renderer->VerifyPixelSampler(1, pLightMap, lightSampler);
	
		auto newWorld = drawCalls[i].data.position.GetTransform();
		if (transformBuffer.world != newWorld) {
			transformBuffer.world = drawCalls[i].data.position.GetTransform();
			renderer->SetConstBuffer(pTransformCB, &transformBuffer);
		}
		dataBuffer.color = drawCalls[i].data.light;
		//
		renderer->SetConstBuffer(pDataCB, &dataBuffer);

		if (drawCalls[i].data.dynamic) {
			renderer->ApplyVertexBufferBinding(&dynamicMeshes.vertexBuffer);
			renderer->DrawIndexedPrimitives(
				drawCalls[i].model.pt, 0, 0, 0, drawCalls[i].model.indexOffset,
				drawCalls[i].model.numElem, dynamicMeshes.indexBuffer, 32);
		}
		else {
			renderer->ApplyVertexBufferBinding(&staticMeshes.vertexBuffer);
			renderer->DrawIndexedPrimitives(
				drawCalls[i].model.pt, 0, 0, 0, drawCalls[i].model.indexOffset,
				drawCalls[i].model.numElem, staticMeshes.indexBuffer, 32);
		}

	}


}

void UPRenderer::Flush() {
	staticMeshes.Flush();
	dynamicMeshes.Flush();
}

void UPRenderer::Clear() {
	drawCalls.clear();
}

void UPRenderer::Destroy() {

	renderer->AddDisposeConstBuffer(pTransformCB);
	renderer->AddDisposeConstBuffer(pDataCB);
	//delete provider;
	delete factory;
}


UPRenderer::~UPRenderer() { Destroy(); }

UPRenderer::DrawCall::DrawCall(MeshHashData model, TexturesManager::TextureCache texture, UPDrawData data) :
	model(model), texture(texture), data(data) {}

UPRenderer::DrawCall::DrawCall(MeshHashData model, TexturesManager::TextureCache texture, TexturesManager::TextureCache lightMap, UPDrawData data) :
	DrawCall(model, texture, data) {
	this->lightMap = lightMap;
}
