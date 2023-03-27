#include "pch.h"
#include "ParticlesRenderPass.h"

#include "RendererPasses/TAARenderPass/TAARenderPassProvider.h"

using namespace Renderer;

ParticlesRenderPass::ParticlesRenderPass(BaseRenderSystem& renderSystem) :
BaseRenderPass({"ParticlesShader.hlsl",renderSystem}),
particlesBuffer(renderSystem.pRenderer, 1000, 200)
{
	
}


void ParticlesRenderPass::Init(const char* dirr)
{
	uint32_t width, height;
	renderDevice->GetMainViewportSize(width, height);
	BaseRenderPass::Init(dirr, new ParticlesRendererProvider(width, height));
    
}

void ParticlesRenderPass::Draw(const ParticlesMesh& particles, const ParticlesDrawData& data) {
	drawCalls.emplace_back(particlesBuffer.AddMesh(particles), data);
}

void ParticlesRenderPass::Render() {
	if (!drawCalls.empty()) {
		particlesBuffer.UpdateBuffers(true);
		
		uint32_t width, height;
		renderDevice->GetMainViewportSize(width, height);
		
		size_t lastFlags = -1;

		RenderTargetBinding* targets[] = {
		baseRendererParams.renderSystem.texturesManger->GetRenderTarget(SID("alphaSurfaces")),
		};
		renderDevice->SetRenderTargets(targets, 1, baseRendererParams.renderSystem.texturesManger->depthBuffer);
		
		renderDevice->ApplyMeshBuffersBinding(particlesBuffer.mesh);

		for (size_t i = 0; i < drawCalls.size(); i++) {
			if (drawCalls[i].data.flags != lastFlags) {
				renderDevice->ApplyPipelineState(factory->GetState({drawCalls[i].data.flags, (uint8_t)UseGeometryShader}));
			}
			lastFlags = drawCalls[i].data.flags;


			renderDevice->DrawIndexedPrimitives(
				(Renderer::PrimitiveType)drawCalls[i].particles.pt, 0, 0, 0, drawCalls[i].particles.indexOffset,
				drawCalls[i].particles.numElem);

		}
	}

}

void ParticlesRenderPass::Clear() {
	drawCalls.clear();
	particlesBuffer.Flush();
}

ParticlesRenderPass::~ParticlesRenderPass()
{
	Destroy();
}

void ParticlesRenderPass::Destroy() {
	delete factory;
}