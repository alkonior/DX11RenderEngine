#pragma once
#include "ParticlesRendererFactory.h"
#include "ParticlesUtils.h"
#include "ParticlesConstBuffer.h"
#include "GraphicsBase.h"



struct ParticlesDrawData {
	uint64_t flags;
};

class ParticlesRenderer {
	struct ParticlesRendererProvider;

	Renderer::IRenderer* renderer;
	ParticlesRendererFactory* factory = nullptr;
	ParticlesRendererProvider* provider = nullptr;

	struct ParticlesRendererProvider : public Renderer::IStateProvider {
		int32_t width, height;
		ParticlesRendererProvider(int32_t width, int32_t  height);
		virtual void PatchPipelineState(Renderer::PipelineState* refToPS, size_t definesFlags) override;
		virtual  Renderer::InputLayoutDescription GetInputLayoutDescription(size_t definesFlags) override;
		virtual ~ParticlesRendererProvider() override;
	};


	struct DrawCall {
		DrawCall(const MeshHashData& particles, const ParticlesDrawData& data) : particles(particles), data(data) {};
		MeshHashData particles;
		ParticlesDrawData data;
	};


public:

	ParticlesRenderer();

	void Init(void* shaderData, size_t dataSize);
	void Init(LPCWSTR dirr);

	void Draw(const ParticlesMesh& particles, const ParticlesDrawData& data);
	
	void Render(const GraphicsBase& gfx);
	void Clear();

	~ParticlesRenderer();

private:

	void Destroy();

	Renderer::Viewport vp;
	DynamicMeshBuffer<ParticleVertex> particlesBuffer;


	ParticlesCosntBuffer constBuffer;

	Renderer::ConstBuffer* pDataCB;

	std::vector<DrawCall> drawCalls;

};

