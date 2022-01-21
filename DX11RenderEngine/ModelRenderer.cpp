#include "ModelRenderer.h"

using namespace Renderer;

ModelRenderer::DrawCall::DrawCall(ModelsManager::ModelCache model, TexturesManager::TextureCache texture, Transform position, size_t flags):
model(model), texture(texture), position(position), flags(flags)
{}

ModelRenderer::ModelRenderer(Renderer::IRenderer* renderer) : renderer(renderer) {}

void ModelRenderer::Init(void* shaderData, size_t dataSize) {
	if (provider != nullptr) {
		delete provider;
		delete factory;
		int32_t width, height;
		renderer->GetBackbufferSize(&width, &height);
		provider = new ModelRendererProvider(width, height);
		factory = new ModelRendererFactory(renderer, provider, shaderData, dataSize);
		return;
	}

	int32_t width, height;
	renderer->GetBackbufferSize(&width, &height);
	provider = new ModelRendererProvider(width, height);
	factory = new ModelRendererFactory(renderer, provider, shaderData, dataSize);


	constBuffer = renderer->CreateConstBuffer(sizeof(localBuffer));


	sampler.filter = TextureFilter::TEXTUREFILTER_LINEAR;
	sampler.addressU = TextureAddressMode::TEXTUREADDRESSMODE_WRAP;
	sampler.addressV = TextureAddressMode::TEXTUREADDRESSMODE_WRAP;
	sampler.addressW = TextureAddressMode::TEXTUREADDRESSMODE_WRAP;

}

void ModelRenderer::Init(LPCWSTR dirr) {
	wrl::ComPtr<ID3DBlob> buff;
	D3DReadFileToBlob((std::wstring(dirr) + L"\\Shader3D.hlsl").c_str(), &buff);
	auto data = buff->GetBufferPointer();
	auto size = buff->GetBufferSize();
	Init(data, size);
}

void ModelRenderer::Draw(ModelsManager::ModelCache model, TexturesManager::TextureCache texture, Transform position, size_t flags) {
	drawCalls.emplace_back(model, texture, position, flags);
}

void ModelRenderer::Render(const GraphicsBase& gfx) {
	int32_t width, height;
	renderer->GetBackbufferSize(&width, &height);
	uint32_t lastFlags = -1;

	renderer->VerifyConstBuffers(&constBuffer, 1);
	renderer->SetRenderTargets(NULL, 0, NULL, DepthFormat::DEPTHFORMAT_NONE, 0);


	for (size_t i = 0; i < drawCalls.size(); i++) {
		if (drawCalls[i].flags != lastFlags) {
			renderer->ApplyPipelineState(factory->GetState(drawCalls[i].flags));
		}


		lastFlags = drawCalls[i].flags;


		renderer->ApplyVertexBufferBinding(drawCalls[i].model.vertexBuffer);

		auto  pTexture = drawCalls[i].texture.texture;
		renderer->VerifyPixelSampler(0, pTexture, sampler);

		localBuffer.world = drawCalls[i].position.GetTransform();
		localBuffer.view = gfx.camera.GetInverseTransform();
		localBuffer.projection = gfx.cameraProjection.Transpose();

		//localBuffer.transform = drawCalls[i].getTransform(width, height).Transpose();
		//localBuffer.uvShift = drawCalls[i].getUVShift();
		//localBuffer.uvScale = drawCalls[i].getUVScale();
		renderer->SetConstBuffer(constBuffer, &localBuffer);
		renderer->DrawIndexedPrimitives(
			drawCalls[i].model.pt, 0, 0, 0, 0, 
			drawCalls[i].model.primitiveCount, drawCalls[i].model.indexBuffer, drawCalls[i].model.indexBufferElementSize);
	}
	drawCalls.clear();
}

ModelRenderer::ModelRendererProvider::ModelRendererProvider(int32_t width, int32_t height):width(width), height(height) {}

void ModelRenderer::ModelRendererProvider::PatchPipelineState(Renderer::PipelineState* refToPS) {


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



	refToPS->rs.cullMode = CullMode::CULLMODE_CULLCLOCKWISEFACE;
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
