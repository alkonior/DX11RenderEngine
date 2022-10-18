#include "pch.h"
#include "BeamRenderer.h"

using namespace Renderer;

BeamRenderer::BeamRenderer() {}


void BeamRenderer::Init(void* shaderData, size_t dataSize) {
	if (provider != nullptr) {
		//delete provider;
		delete factory;
		int32_t width, height;
		renderer->GetBackbufferSize(&width, &height);
		provider = new BeamRendererProvider(width, height);
		factory = new BeamRendererFactory(provider, shaderData, dataSize);
		return;
	}

	int32_t width, height;
	renderer->GetBackbufferSize(&width, &height);
	provider = new BeamRendererProvider(width, height);
	factory = new BeamRendererFactory(provider, shaderData, dataSize);
	

	vp.x = 0;
	vp.y = 0;
	vp.w = width;
	vp.h = height;
	vp.minDepth = 0.0f;
	vp.maxDepth = 1.0f;
}

void BeamRenderer::Init(LPCWSTR dirr) {
	//wrl::ComPtr<ID3DBlob> buff;
	//D3DReadFileToBlob((std::wstring(dirr) + L"\\BeamShader.hlsl").c_str(), &buff);
	//auto data = buff->GetBufferPointer();
	//auto size = buff->GetBufferSize();
	//Init(data, size);
}

void BeamRenderer::Draw(const BeamDrawData& data) {
	//drawCalls.emplace_back(particlesBuffer.AddMesh(particles), data);
}

void BeamRenderer::Render(const GraphicsBase& gfx) {
	if (!drawCalls.empty()) {
		
		int32_t width, height;
		renderer->GetBackbufferSize(&width, &height);
		size_t lastFlags = -1;

		RenderTargetBinding* targets[] = { (RenderTargetBinding*)&gfx.texturesManger.alphaSurfacesRT };
		renderer->SetRenderTargets(targets, 1, gfx.texturesManger.depthBuffer, vp);

		for (size_t i = 0; i < drawCalls.size(); i++) {
			if (drawCalls[i].data.flags != lastFlags) {
				renderer->ApplyPipelineState(factory->GetState(drawCalls[i].data.flags));
			}
			lastFlags = drawCalls[i].data.flags;


			//renderer->DrawIndexedPrimitives(
			//drawCalls[i].particles.pt, 0, 0, 0, drawCalls[i].particles.indexOffset,
			//	drawCalls[i].particles.numElem);

		}
	}

}

void BeamRenderer::Clear() {
	drawCalls.clear();
	//particlesBuffer.Flush();
}

BeamRenderer::~BeamRenderer()
{
	Destroy();
}

void BeamRenderer::Destroy() {
	delete factory;
}