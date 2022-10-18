#include "pch.h"
#include "ModelRenderer.h"

#include <d3d11.h>
#include <dxgiformat.h>

#include "CoreRenderSystem/ResourceManagers/States/BlendStates.h"
#include "CoreRenderSystem/ResourceManagers/States/DSStates.h"
#include "CoreRenderSystem/ResourceManagers/States/RasterizerStates.h"

using namespace Renderer;

ModelRenderer::DrawCall::DrawCall(ModelsManager::ModelCache model, TexturesManager::TextureCache texture, Transform position, size_t flags) :
	model(model), texture(texture), position(position), flags(flags) {}

ModelRenderer::DrawLerpCall::DrawLerpCall(ModelsManager::ModelCache model, TexturesManager::TextureCache texture, const LerpModelDrawData& data) :
	model(model), texture(texture), data(data) {}

ModelRenderer::ModelRenderer() : BaseRenderer("ModelsShader.hlsl") {}

void ModelRenderer::Init(void* shaderData, size_t dataSize) {
	if (provider != nullptr) {
		//delete provider;
		delete factory;
		int32_t width, height;
		renderer->GetBackbufferSize(&width, &height);
		provider = new ModelRendererProvider(width, height);
		factory = new ModelRendererFactory(provider, shaderData, dataSize);
		return;
	}

	int32_t width, height;
	renderer->GetBackbufferSize(&width, &height);
	provider = new ModelRendererProvider(width, height);
	factory = new ModelRendererFactory(provider, shaderData, dataSize);



	vp.x = 0;
	vp.y = 0;
	vp.w = width;
	vp.h = height;
	vp.minDepth = 0.0f;
	vp.maxDepth = 1.0f;

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

	renderer->AddDisposeConstBuffer(pDataCB);
	//delete provider;
	delete factory;
}

void ModelRenderer::Render(GraphicsBase& gfx) {
	int32_t width, height;
	renderer->GetBackbufferSize(&width, &height);

	RenderTargetBinding* targets[5] = {
		&gfx.texturesManger.diffuseColorRT,
		&gfx.texturesManger.lightColorRT,
		&gfx.texturesManger.velocityFieldRT,
		&gfx.texturesManger.blurMaskRT,
		&gfx.texturesManger.normalsFieldRT,
   };
	
	renderer->SetRenderTargets(targets, std::size(targets), gfx.texturesManger.depthBuffer, vp);


	size_t lastFlags = -1;

	renderer->VerifyConstBuffer(pDataCB, modelsCosntBuffer.slot);


	renderer->VerifyPixelSampler(0, Samplers::anisotropic16);


	for (size_t i = 0; i < drawCalls.size(); i++) {
		if (drawCalls[i].flags != lastFlags) {
			renderer->ApplyPipelineState(factory->GetState(drawCalls[i].flags));
			lastFlags = drawCalls[i].flags;
		}

		renderer->ApplyVertexBufferBinding(drawCalls[i].model.vertexBuffer);
		renderer->ApplyIndexBufferBinding(drawLerpCalls[i].model.indexBuffer, drawLerpCalls[i].model.indexBufferElementSize);

		auto  pTexture = drawCalls[i].texture.texture;
		renderer->VerifyPixelTexture(0, pTexture);

		renderer->DrawIndexedPrimitives(
			drawCalls[i].model.pt, 0, 0, 0, 0,
			drawCalls[i].model.primitiveCount);
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
		int max_buff = drawLerpCalls[i].model.vertexBuffer.buffersCount - 1;
		vertexBuffers[0] = drawLerpCalls[i].model.vertexBuffer.vertexBuffers[0];
		vertexBuffers[1] = drawLerpCalls[i].model.vertexBuffer.vertexBuffers[drawLerpCalls[i].data.currentFrame % max_buff + 1];
		vertexBuffers[2] = drawLerpCalls[i].model.vertexBuffer.vertexBuffers[drawLerpCalls[i].data.nextFrame % max_buff + 1];

		strides[0] = drawLerpCalls[i].model.vertexBuffer.vertexStride[0];
		strides[1] = drawLerpCalls[i].model.vertexBuffer.vertexStride[drawLerpCalls[i].data.currentFrame + 1];
		strides[2] = drawLerpCalls[i].model.vertexBuffer.vertexStride[drawLerpCalls[i].data.nextFrame + 1];

		vBB.vertexOffset = ofsets;
		vBB.vertexStride = strides;

		renderer->ApplyVertexBufferBinding(vBB);
		renderer->ApplyIndexBufferBinding(drawLerpCalls[i].model.indexBuffer, drawLerpCalls[i].model.indexBufferElementSize);

		auto  pTexture = drawLerpCalls[i].texture.texture;
		renderer->VerifyPixelTexture(0, pTexture);


		dataBuffer.alpha = drawLerpCalls[i].data.alpha;
		dataBuffer.oldAlpha = drawLerpCalls[i].data.oldAlpha;
		dataBuffer.wh = float2(drawLerpCalls[i].texture.width, drawLerpCalls[i].texture.height);
		dataBuffer.color = drawLerpCalls[i].data.color;
		dataBuffer.world = drawLerpCalls[i].data.newPosition.GetTransform();
		dataBuffer.oldWorld = drawLerpCalls[i].data.oldPosition.GetTransform();
		dataBuffer.blurSwitch = 2.f;
		if (drawLerpCalls[i].data.isGun)
			dataBuffer.blurSwitch = 1.f;
		renderer->SetConstBuffer(pDataCB, &dataBuffer);



		renderer->DrawIndexedPrimitives(
			drawLerpCalls[i].model.pt, 0, 0, 0, 0,
			drawLerpCalls[i].model.primitiveCount);
	}


}

ModelRenderer::ModelRendererProvider::ModelRendererProvider(int32_t width, int32_t height) :width(width), height(height) {}

void ModelRenderer::ModelRendererProvider::PatchPipelineState(Renderer::PipelineState* refToPS, size_t definesFlags) {

	refToPS->bs = &BlendStates::NoAlpha;

	refToPS->dss = &DepthStencilStates::DSS;

	refToPS->rs = &RasterizerStates::CClockWise;


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

const char* ModelRenderer::ModelRendererProvider::GetShaderName()
{
	return "Modles shader";
}


