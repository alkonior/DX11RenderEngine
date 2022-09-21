#pragma once
#include "DynamicMeshBuffer.h"
#include "ParticlesRendererFactory.h"
#include "ParticlesConstBuffer.h"
#include "CoreRenderSystem/Renderers/BaseRenderer.h"





class ParticlesRenderer : public BaseRenderer {
	struct ParticlesRendererProvider;

	ParticlesRendererFactory* factory = nullptr;
	ParticlesRendererProvider* provider = nullptr;

	struct ParticlesRendererProvider : public Renderer::IStateProvider {
		int32_t width, height;
		ParticlesRendererProvider(int32_t width, int32_t  height);
		virtual void PatchPipelineState(Renderer::PipelineState* refToPS, size_t definesFlags) override;
		virtual  Renderer::InputLayoutDescription GetInputLayoutDescription(size_t definesFlags) override;
		virtual const char* GetShaderName() override;
	};


	struct DrawCall {
		DrawCall(const MeshHashData& particles, const ParticlesDrawData& data) : particles(particles), data(data) {};
		MeshHashData particles;
		ParticlesDrawData data;
	};


public:

	ParticlesRenderer();

	virtual  void Init(void* shaderData, size_t dataSize) override;
	

	void Draw(const ParticlesMesh& particles, const ParticlesDrawData& data);
	
	void Render(const GraphicsBase& gfx);
	void Clear();

	~ParticlesRenderer() override;

private:

	void Destroy();

	Renderer::Viewport vp;
	DynamicMeshBuffer<ParticleVertex> particlesBuffer;



	std::vector<DrawCall> drawCalls;

};

