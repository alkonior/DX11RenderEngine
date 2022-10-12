#include "pch.h"
#include "ParticlesRenderer.h"

using namespace Renderer;

ParticlesRenderer::ParticlesRenderer() : BaseRenderer("ParticlesShader.hlsl"),
particlesBuffer(1000, 200) {}


void ParticlesRenderer::Init(void* shaderData, size_t dataSize) {
	if (provider != nullptr) {
		//delete provider;
		delete factory;
		int32_t width, height;
		renderer->GetBackbufferSize(&width, &height);
		provider = new ParticlesRendererProvider(width, height);
		factory = new ParticlesRendererFactory(provider, shaderData, dataSize);
		return;
	}

	int32_t width, height;
	renderer->GetBackbufferSize(&width, &height);
	provider = new ParticlesRendererProvider(width, height);
	factory = new ParticlesRendererFactory(provider, shaderData, dataSize);
	

	vp.x = 0;
	vp.y = 0;
	vp.w = width;
	vp.h = height;
	vp.minDepth = 0.0f;
	vp.maxDepth = 1.0f;
}

void ParticlesRenderer::Draw(const ParticlesMesh& particles, const ParticlesDrawData& data) {
	drawCalls.emplace_back(particlesBuffer.AddMesh(particles), data);
}

void ParticlesRenderer::Render(const GraphicsBase& gfx) {
	if (!drawCalls.empty()) {
		particlesBuffer.UpdateBuffers(true);
		
		int32_t width, height;
		renderer->GetBackbufferSize(&width, &height);
		size_t lastFlags = -1;

		RenderTargetBinding* targets[] = { (RenderTargetBinding*)&gfx.texturesManger.alphaSurfacesRT };
		renderer->SetRenderTargets(targets, 1, gfx.texturesManger.depthBuffer, vp);
		
		renderer->ApplyMeshBuffersBinding(particlesBuffer.mesh);

		for (size_t i = 0; i < drawCalls.size(); i++) {
			if (drawCalls[i].data.flags != lastFlags) {
				renderer->ApplyPipelineState(factory->GetState(drawCalls[i].data.flags));
			}
			lastFlags = drawCalls[i].data.flags;


			renderer->DrawIndexedPrimitives(
				(Renderer::PrimitiveType)drawCalls[i].particles.pt, 0, 0, 0, drawCalls[i].particles.indexOffset,
				drawCalls[i].particles.numElem);

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
	delete factory;
}