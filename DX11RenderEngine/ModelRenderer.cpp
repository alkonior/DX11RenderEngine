#include "ModelRenderer.h"

using namespace Renderer;

ModelRenderer::DrawCall::DrawCall(ModelsManager::ModelCache model, TexturesManager::TextureCache texture, Transform position, size_t flags) :
	model(model), texture(texture), position(position), flags(flags) {}

ModelRenderer::DrawLerpCall::DrawLerpCall(ModelsManager::ModelCache model, TexturesManager::TextureCache texture, const LerpModelDrawData& data) :
	model(model), texture(texture), data(data) {}

ModelRenderer::ModelRenderer(Renderer::IRenderer* renderer) : renderer(renderer) {}

void ModelRenderer::Init(void* shaderData, size_t dataSize) {
	if (provider != nullptr) {
		//delete provider;
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


	pTransformCB = renderer->CreateConstBuffer(sizeof(transformBuffer));
	pDataCB = renderer->CreateConstBuffer(sizeof(dataBuffer));


	sampler.filter = TextureFilter::TEXTUREFILTER_POINT;
	sampler.addressU = TextureAddressMode::TEXTUREADDRESSMODE_WRAP;
	sampler.addressV = TextureAddressMode::TEXTUREADDRESSMODE_WRAP;
	sampler.addressW = TextureAddressMode::TEXTUREADDRESSMODE_WRAP;
	sampler.mipMapLevelOfDetailBias = 0;
}

void ModelRenderer::Init(LPCWSTR dirr) {
	wrl::ComPtr<ID3DBlob> buff;
	D3DReadFileToBlob((std::wstring(dirr) + L"\\ModelsShader.hlsl").c_str(), &buff);
	auto data = buff->GetBufferPointer();
	auto size = buff->GetBufferSize();
	Init(data, size);
}

void ModelRenderer::Draw(ModelsManager::ModelCache model, TexturesManager::TextureCache texture, Transform position, size_t flags) {
	drawCalls.emplace_back(model, texture, position, flags);
}

void ModelRenderer::DrawLerp(ModelsManager::ModelCache model, TexturesManager::TextureCache texture, const LerpModelDrawData& data) {
	drawLerpCalls.emplace_back(model, texture, data);
}

void ModelRenderer::Clear() {

	drawCalls.clear();
	drawLerpCalls.clear();
}

ModelRenderer::~ModelRenderer() { Destroy(); }

void ModelRenderer::Destroy() {

	renderer->AddDisposeConstBuffer(pTransformCB);
	renderer->AddDisposeConstBuffer(pDataCB);
	delete provider;
	delete factory;
}

void ModelRenderer::Render(const GraphicsBase& gfx) {
	int32_t width, height;
	renderer->GetBackbufferSize(&width, &height);
	size_t lastFlags = -1;

	renderer->VerifyConstBuffer(pTransformCB, ModelsTransform.slot);
	renderer->VerifyConstBuffer(pDataCB, ModelsExtraData.slot);

	renderer->SetRenderTargets(NULL, 0, NULL, DepthFormat::DEPTHFORMAT_NONE, 0);

	transformBuffer.view = gfx.camera;
	transformBuffer.projection = gfx.cameraProjection;

	for (size_t i = 0; i < drawCalls.size(); i++) {
		if (drawCalls[i].flags != lastFlags) {
			renderer->ApplyPipelineState(factory->GetState(drawCalls[i].flags));
			lastFlags = drawCalls[i].flags;
		}




		renderer->ApplyVertexBufferBinding(drawCalls[i].model.vertexBuffer);

		auto  pTexture = drawCalls[i].texture.texture;
		renderer->VerifyPixelSampler(0, pTexture, sampler);

		transformBuffer.world = drawCalls[i].position.GetTransform();

		//localBuffer.transform = drawCalls[i].getTransform(width, height).Transpose();
		//localBuffer.uvShift = drawCalls[i].getUVShift();
		//localBuffer.uvScale = drawCalls[i].getUVScale();
		renderer->SetConstBuffer(pTransformCB, &transformBuffer);
		renderer->DrawIndexedPrimitives(
			drawCalls[i].model.pt, 0, 0, 0, 0,
			drawCalls[i].model.primitiveCount, drawCalls[i].model.indexBuffer, drawCalls[i].model.indexBufferElementSize);
	}

	for (size_t i = 0; i < drawLerpCalls.size(); i++) {
		if (drawLerpCalls[i].data.flags != lastFlags) {
			renderer->ApplyPipelineState(factory->GetState(drawLerpCalls[i].data.flags));
			lastFlags = drawLerpCalls[i].data.flags;
		}


		static Buffer* vertexBuffers[3];
		static UINT ofsets[3] = { 0, 0, 0 };
		static UINT strides[3] = { 0, 0, 0 };
		static VertexBufferBinding vBB;

		vBB.buffersCount = 3;
		if (drawLerpCalls[i].data.isSingle)
			vBB.buffersCount = 2;
		vBB.vertexBuffers = vertexBuffers;
		int max_buff = drawLerpCalls[i].model.vertexBuffer->buffersCount - 1;
		vertexBuffers[0] = drawLerpCalls[i].model.vertexBuffer->vertexBuffers[0];
		vertexBuffers[1] = drawLerpCalls[i].model.vertexBuffer->vertexBuffers[drawLerpCalls[i].data.currentFrame % max_buff + 1];
		vertexBuffers[2] = drawLerpCalls[i].model.vertexBuffer->vertexBuffers[drawLerpCalls[i].data.nextFrame % max_buff + 1];

		strides[0] = drawLerpCalls[i].model.vertexBuffer->vertexStride[0];
		strides[1] = drawLerpCalls[i].model.vertexBuffer->vertexStride[drawLerpCalls[i].data.currentFrame + 1];
		strides[2] = drawLerpCalls[i].model.vertexBuffer->vertexStride[drawLerpCalls[i].data.nextFrame + 1];

		vBB.vertexOffset = ofsets;
		vBB.vertexStride = strides;

		renderer->ApplyVertexBufferBinding(&vBB);

		auto  pTexture = drawLerpCalls[i].texture.texture;
		renderer->VerifyPixelSampler(0, pTexture, sampler);

		transformBuffer.world = drawLerpCalls[i].data.position.GetTransform();

		dataBuffer.alpha = drawLerpCalls[i].data.alpha;
		dataBuffer.wh = float2(drawLerpCalls[i].texture.width, drawLerpCalls[i].texture.height);
		dataBuffer.color = drawLerpCalls[i].data.color;
		//localBuffer.transform = drawCalls[i].getTransform(width, height).Transpose();
		//localBuffer.uvShift = drawCalls[i].getUVShift();
		//localBuffer.uvScale = drawCalls[i].getUVScale();
		renderer->SetConstBuffer(pTransformCB, &transformBuffer);
		renderer->SetConstBuffer(pDataCB, &dataBuffer);



		renderer->DrawIndexedPrimitives(
			drawLerpCalls[i].model.pt, 0, 0, 0, 0,
			drawLerpCalls[i].model.primitiveCount, drawLerpCalls[i].model.indexBuffer, drawLerpCalls[i].model.indexBufferElementSize);
	}


}

ModelRenderer::ModelRendererProvider::ModelRendererProvider(int32_t width, int32_t height) :width(width), height(height) {}

void ModelRenderer::ModelRendererProvider::PatchPipelineState(Renderer::PipelineState* refToPS, size_t definesFlags) {

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


	refToPS->dss.depthBufferEnable = true;
	refToPS->dss.depthBufferFunction = CompareFunction::COMPAREFUNCTION_LESSEQUAL;
	refToPS->dss.stencilEnable = false;


	refToPS->rs.cullMode = CullMode::CULLMODE_CULLCLOCKWISEFACE;

	if (definesFlags & ModelDefines::MBAD_UV)
		refToPS->rs.cullMode = CullMode::CULLMODE_CULLCOUNTERCLOCKWISEFACE;
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

const D3D11_INPUT_ELEMENT_DESC  DefaultInputElements[] =
{
	{ "Position",    0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL",      0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD",    0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

const D3D11_INPUT_ELEMENT_DESC  LerpInputElements[] =
{
	{ "TEXCOORD",      0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "Position",      1, DXGI_FORMAT_R32G32B32_FLOAT,    1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL",        1, DXGI_FORMAT_R32G32B32_FLOAT,    1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "Position",      2, DXGI_FORMAT_R32G32B32_FLOAT,    2, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL",        2, DXGI_FORMAT_R32G32B32_FLOAT,    2, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};


const D3D11_INPUT_ELEMENT_DESC  SingleFrameInputElements[] =
{
	{ "Position",           0, DXGI_FORMAT_R32G32B32_FLOAT,    1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL",             0, DXGI_FORMAT_R32G32B32_FLOAT,    1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD",           0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

InputLayoutDescription ModelRenderer::ModelRendererProvider::GetInputLayoutDescription(size_t definesFlags) {
	if (definesFlags & ModelDefines::MLERP) {
		return InputLayoutDescription{ (void*)LerpInputElements, std::size(LerpInputElements) };
	}
	else {
		return InputLayoutDescription{ (void*)SingleFrameInputElements, std::size(SingleFrameInputElements) };
	}

	//return InputLayoutDescription{ (void*)DefaultInputElements, std::size(DefaultInputElements) };

}

ModelRenderer::ModelRendererProvider::~ModelRendererProvider() {}

