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


	//vertexBuffer.buffersCount = 1;
	//vertexBuffer.vertexBuffers = new Buffer * [1]();

	//bigVertexBuffer.buffersCount = 1;
	//bigVertexBuffer.vertexBuffers = new Buffer * [1]();
	//bigVertexBuffer.vertexBuffers[0] = renderer->GenVertexBuffer(1, BufferUsage::BUFFERUSAGE_WRITEONLY, sizeof(UPVertex) * bigVertexBuffCapacity);
	//bigIndexBuffer = renderer->GenIndexBuffer(1, BufferUsage::BUFFERUSAGE_WRITEONLY, sizeof(uint32_t) * bigIndexBuffCapacity);

	pTransformCB = renderer->CreateConstBuffer(sizeof(transformBuffer));
	//pDataCB = renderer->CreateConstBuffer(sizeof(dataBuffer));

	//vertexBuffer.vertexOffset = new (UINT)(0);
	//vertexBuffer.vertexStride = new (UINT)(sizeof(UPVertex));

	//bigVertexBuffer.vertexOffset = new (UINT)(0);
	//bigVertexBuffer.vertexStride = new (UINT)(sizeof(UPVertex));


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

	//MeshHashData res{ cpuVertices.size(), cpuIndexes.size(),  model.primitiveCount, model.pt, dynamic };
	//
	//for (size_t i = 0; i < model.indexes.size(); i++) {
	//	cpuIndexes.push_back(model.indexes[i] + cpuVertices.size());
	//}
	//
	//for (size_t i = 0; i < model.verticies.size(); i++) {
	//	cpuVertices.push_back(model.verticies[i]);
	//}

	if (dynamic) {
		return dynamicMeshes.AddMesh(model);
	}
	else {
		return staticMeshes.AddMesh(model);
	}


	//if (cpuVerticies.size() < bigVertexBuffCapacity)
	//	renderer->SetVertexBufferData(bigVertexBuffer.vertexBuffers[0], res.vertexOffset * sizeof(UPVertex), model.verticies.data(), model.verticies.size(), sizeof(UPVertex), sizeof(UPVertex), Renderer::SetDataOptions::SETDATAOPTIONS_NOOVERWRITE);
	//if (cpuIndexes.size() < bigIndexBuffCapacity)
	//	renderer->SetIndexBufferData(bigIndexBuffer, res.indexOffset * sizeof(std::uint32_t), model.indexes.data(), model.indexes.size() * sizeof(std::uint32_t), Renderer::SetDataOptions::SETDATAOPTIONS_NOOVERWRITE);

	//bigVertexBuffSize = cpuVertices.size();
	//bigIndexBuffSize = cpuIndexes.size();

	//return res;

}

void UPRenderer::Draw(MeshHashData model, TexturesManager::TextureCache texture, UPDrawData data) {
	drawCalls.emplace_back(model, texture, data);
}
void UPRenderer::Draw(MeshHashData model, TexturesManager::TextureCache texture, TexturesManager::TextureCache lightMap, UPDrawData data) {
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

	//if (bigVertexBuffSize < bigVertexBuffCapacity)
	//	renderer->SetVertexBufferData(bigVertexBuffer.vertexBuffers[0], model.vertexOffset * sizeof(UPVertex), newModel.verticies.data(), newModel.verticies.size(), sizeof(UPVertex), sizeof(UPVertex), Renderer::SetDataOptions::SETDATAOPTIONS_NOOVERWRITE);
	//if (bigIndexBuffSize < bigIndexBuffCapacity)
	//	renderer->SetIndexBufferData(bigIndexBuffer, model.indexOffset*sizeof(std::uint32_t), newModel.indexes.data(), newModel.indexes.size() * sizeof(std::uint32_t), Renderer::SetDataOptions::SETDATAOPTIONS_NOOVERWRITE);

	drawCalls.emplace_back(model, texture, data);
}

void UPRenderer::Render(const GraphicsBase& gfx) {


	//if (bigVertexBuffCapacity <= bigVertexBuffSize) {
	//	bigVertexBuffCapacity = bigVertexBuffSize;
	//	ResizeBigVertexBuffer(std::floor(bigVertexBuffCapacity * 1.3));
	//	//	renderer->SetVertexBufferData(bigVertexBuffer.vertexBuffers[0], 0, cpuVerticies.data(), cpuVerticies.size(), sizeof(UPVertex), sizeof(UPVertex), Renderer::SetDataOptions::SETDATAOPTIONS_DISCARD);
	//}
	//if (bigIndexBuffCapacity <= bigIndexBuffSize) {
	//	bigIndexBuffCapacity = bigIndexBuffSize;
	//	ResizeBigIndexBuffer(std::floor(bigIndexBuffCapacity * 1.3));
	//	//	renderer->SetIndexBufferData(bigIndexBuffer, 0, cpuIndexes.data(), cpuIndexes.size() * sizeof(std::uint32_t), Renderer::SetDataOptions::SETDATAOPTIONS_DISCARD);
	//}
	//
	//
	//renderer->SetIndexBufferData(bigIndexBuffer, 0, cpuIndexes.data(), cpuIndexes.size() * sizeof(std::uint32_t), Renderer::SetDataOptions::SETDATAOPTIONS_DISCARD);
	//renderer->SetVertexBufferData(bigVertexBuffer.vertexBuffers[0], 0, cpuVertices.data(), cpuVertices.size(), sizeof(UPVertex), sizeof(UPVertex), Renderer::SetDataOptions::SETDATAOPTIONS_DISCARD);


	//ResizeBuffers(drawCalls[i].data.model.verticies.size(), drawCalls[i].data.model.indexes.size());
	//renderer->SetVertexBufferData(vertexBuffer.vertexBuffers[0], 0, drawCalls[i].data.model.verticies.data(),
	//	drawCalls[i].data.model.verticies.size(), sizeof(UPVertex), sizeof(UPVertex), Renderer::SetDataOptions::SETDATAOPTIONS_DISCARD);
	//renderer->SetIndexBufferData(indexBuffer, 0, drawCalls[i].data.model.indexes.data(), drawCalls[i].data.model.indexes.size() * 2, Renderer::SetDataOptions::SETDATAOPTIONS_DISCARD);


	//bigVertexBuffSize = cpuVertices.size();
	//bigIndexBuffSize = cpuIndexes.size();
	

	staticMeshes.UpdateBuffers();
	dynamicMeshes.UpdateBuffers(true);



	int32_t width, height;
	renderer->GetBackbufferSize(&width, &height);
	size_t lastFlags = -1;

	//ConstBuffer* constBuffers[] = { pTransformCB , pDataCB };
	//renderer->VerifyConstBuffers(constBuffers, 2);
	renderer->VerifyConstBuffers(&pTransformCB, 1);
	renderer->SetRenderTargets(NULL, 0, NULL, DepthFormat::DEPTHFORMAT_NONE, 0);

	transformBuffer.view = gfx.camera;
	transformBuffer.projection = gfx.cameraProjection;

	for (size_t i = 0; i < drawCalls.size(); i++) {
		if (drawCalls[i].data.flags != lastFlags) {
			renderer->ApplyPipelineState(factory->GetState(drawCalls[i].data.flags));
			//renderer->VerifyConstBuffers(constBuffers, 2);
			renderer->VerifyConstBuffers(&pTransformCB, 1);
		}
		lastFlags = drawCalls[i].data.flags;

		//ResizeBuffers(drawCalls[i].data.model.verticies.size(), drawCalls[i].data.model.indexes.size());
		//renderer->SetVertexBufferData(vertexBuffer.vertexBuffers[0], 0, drawCalls[i].data.model.verticies.data(),
		//	drawCalls[i].data.model.verticies.size(), sizeof(UPVertex), sizeof(UPVertex), Renderer::SetDataOptions::SETDATAOPTIONS_DISCARD);
		//renderer->SetIndexBufferData(indexBuffer, 0, drawCalls[i].data.model.indexes.data(), drawCalls[i].data.model.indexes.size() * 2, Renderer::SetDataOptions::SETDATAOPTIONS_DISCARD);

		//

		auto  pTexture = drawCalls[i].texture.texture;
		renderer->VerifyPixelSampler(0, pTexture, sampler);

		auto  pLightMap = drawCalls[i].lightMap.texture;
		if (drawCalls[i].data.flags & UPLIGHTMAPPED)
			renderer->VerifyPixelSampler(1, pLightMap, lightSampler);

		transformBuffer.world = drawCalls[i].data.position.GetTransform();
		transformBuffer.color = drawCalls[i].data.light;
		transformBuffer.texOffset = drawCalls[i].data.texOffset;

		//localBuffer.transform = drawCalls[i].getTransform(width, height).Transpose();
		//localBuffer.uvShift = drawCalls[i].getUVShift();
		//localBuffer.uvScale = drawCalls[i].getUVScale();
		renderer->SetConstBuffer(pTransformCB, &transformBuffer);
		//renderer->SetConstBuffer(pDataCB, &dataBuffer);

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

		//renderer->DrawIndexedPrimitives(
		//	drawCalls[i].model.pt, 0, 0, 0, drawCalls[i].model.indexOffset,
		//	drawCalls[i].model.numElem, bigIndexBuffer, 32);
	}


}

void UPRenderer::Flush() {
	staticMeshes.Flush();
	dynamicMeshes.Flush();
	//cpuVertices.clear();
	//cpuIndexes.clear();
}

void UPRenderer::Clear() {
	drawCalls.clear();
}

void UPRenderer::Destroy() {
	//delete bigVertexBuffer.vertexOffset;
	//delete bigVertexBuffer.vertexStride;
	//renderer->AddDisposeVertexBuffer(bigVertexBuffer.vertexBuffers[0]);
	//renderer->AddDisposeIndexBuffer(bigIndexBuffer);
	//delete[] bigVertexBuffer.vertexBuffers;

	renderer->AddDisposeConstBuffer(pTransformCB);
	renderer->AddDisposeConstBuffer(pDataCB);
	delete provider;
	delete factory;
}


//void UPRenderer::ResizeBigVertexBuffer(size_t newVertexBuffSize) {
//	auto buffVB = bigVertexBuffer.vertexBuffers[0];
//	//std::vector<UPVertex> oldData (bigVertexBuffSize);
//	{
//		//if (vertexBuffer.vertexBuffers[0] != nullptr)
//		bigVertexBuffer.vertexBuffers[0] = renderer->GenVertexBuffer(1, BufferUsage::BUFFERUSAGE_WRITEONLY, newVertexBuffSize * sizeof(UPVertex));
//		//renderer->GetVertexBufferData(buffVB, 0, oldData.data(), bigVertexBuffSize, sizeof(UPVertex), sizeof(UPVertex));
//		//renderer->SetVertexBufferData(bigVertexBuffer.vertexBuffers[0], 0, oldData.data(), bigVertexBuffSize, sizeof(UPVertex), sizeof(UPVertex), Renderer::SetDataOptions::SETDATAOPTIONS_DISCARD);
//		renderer->AddDisposeVertexBuffer(buffVB);
//		bigVertexBuffCapacity = newVertexBuffSize;
//	}
//	//delete[] (UPVertex*)oldData;
//}
//
//void UPRenderer::ResizeBigIndexBuffer(size_t newIndexBuffSize) {
//	auto buffIB = bigIndexBuffer;
//	//std::vector<uint32_t> oldData(bigIndexBuffSize);
//	{
//		bigIndexBuffer = renderer->GenIndexBuffer(1, BufferUsage::BUFFERUSAGE_WRITEONLY, newIndexBuffSize * sizeof(std::uint32_t));
//		//renderer->GetIndexBufferData(buffIB, 0, oldData.data(),  bigIndexBuffSize*sizeof(std::uint32_t));
//		//renderer->SetIndexBufferData(bigVertexBuffer.vertexBuffers[0], 0, oldData.data(), bigIndexBuffSize * sizeof(std::uint32_t), Renderer::SetDataOptions::SETDATAOPTIONS_DISCARD);
//		renderer->AddDisposeIndexBuffer(buffIB);
//		bigIndexBuffCapacity = newIndexBuffSize;
//	}
//	//renderer->SetIndexBufferData(indexBuffer, 0, (void*)indices, 12, SetDataOptions::SETDATAOPTIONS_DISCARD);
//	//delete[] (uint32_t*)oldData;
//}


UPRenderer::~UPRenderer() { Destroy(); }

UPRenderer::DrawCall::DrawCall(MeshHashData model, TexturesManager::TextureCache texture, UPDrawData data) :
	model(model), texture(texture), data(data) {}

UPRenderer::DrawCall::DrawCall(MeshHashData model, TexturesManager::TextureCache texture, TexturesManager::TextureCache lightMap, UPDrawData data) :
	DrawCall(model, texture, data) {
	this->lightMap = lightMap;
}
