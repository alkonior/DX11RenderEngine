#include "ParticlesRenderer.h"

using namespace Renderer;

ParticlesRenderer::ParticlesRenderer(IRenderer* renderer) :
	renderer(renderer), particlesBuffer(renderer, 1000, 200) {}


void ParticlesRenderer::Init(void* shaderData, size_t dataSize) {
	if (provider != nullptr) {
		//delete provider;
		delete factory;
		int32_t width, height;
		renderer->GetBackbufferSize(&width, &height);
		provider = new ParticlesRendererProvider(width, height);
		factory = new ParticlesRendererFactory(renderer, provider, shaderData, dataSize);
		return;
	}

	int32_t width, height;
	renderer->GetBackbufferSize(&width, &height);
	provider = new ParticlesRendererProvider(width, height);
	factory = new ParticlesRendererFactory(renderer, provider, shaderData, dataSize);
	
	pDataCB = renderer->CreateConstBuffer(sizeof(constBuffer));

}

void ParticlesRenderer::Init(LPCWSTR dirr) {
	wrl::ComPtr<ID3DBlob> buff;
	D3DReadFileToBlob((std::wstring(dirr) + L"\\ParticlesShader.hlsl").c_str(), &buff);
	auto data = buff->GetBufferPointer();
	auto size = buff->GetBufferSize();
	Init(data, size);
}

void ParticlesRenderer::Draw(const ParticlesMesh& particles, const ParticlesDrawData& data) {
	drawCalls.emplace_back(particlesBuffer.AddMesh(particles),data);
}

void ParticlesRenderer::Render(const GraphicsBase& gfx) {
	if (!drawCalls.empty()) {
		particlesBuffer.UpdateBuffers(true);

		int32_t width, height;
		renderer->GetBackbufferSize(&width, &height);
		size_t lastFlags = -1;

		renderer->SetRenderTargets(NULL, 0, NULL, DepthFormat::DEPTHFORMAT_NONE, 0);

		constBuffer.view = gfx.viewMatrix;
		constBuffer.projection = gfx.cameraProjection;
		renderer->SetConstBuffer(pDataCB, &constBuffer);

		renderer->VerifyConstBuffer(pDataCB, ParticlesCB.slot);

		for (size_t i = 0; i < drawCalls.size(); i++) {
			if (drawCalls[i].data.flags != lastFlags) {
				renderer->ApplyPipelineState(factory->GetState(drawCalls[i].data.flags));
			}
			lastFlags = drawCalls[i].data.flags;


			renderer->ApplyVertexBufferBinding(&particlesBuffer.vertexBuffer);
			renderer->DrawIndexedPrimitives(
				drawCalls[i].particles.pt, 0, 0, 0, drawCalls[i].particles.indexOffset,
				drawCalls[i].particles.numElem, particlesBuffer.indexBuffer, 32);

		}
	}

}

void ParticlesRenderer::Clear() {
	drawCalls.clear();
	particlesBuffer.Flush();
}

ParticlesRenderer::~ParticlesRenderer()
{
	Destroy();
}

void ParticlesRenderer::Destroy() {
	renderer->AddDisposeConstBuffer(pDataCB);
	delete factory;
}