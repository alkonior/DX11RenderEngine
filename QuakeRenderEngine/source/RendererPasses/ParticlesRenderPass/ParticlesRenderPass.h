#pragma once
#include "CoreRenderSystem/RenderPasses/QuadRenderPass.h"
#include "Utils/DrawData.h"
#include "DynamicMeshBuffer.h"

enum ParticlesDefines : uint64_t {
	PARZERO = 0,
	PARRED = 1,
};

using Renderer::ShaderDefines;

const ShaderDefines ParticlesRendererDefines[] = {
	ShaderDefines("RED"),
};




class ParticlesRenderPass : public BaseRenderPass {

private:
	struct ParticlesRendererProvider;

	ParticlesRendererProvider* provider = nullptr;

	struct ParticlesRendererProvider : public Renderer::IStateProvider {
		int32_t width, height;
		ParticlesRendererProvider(int32_t width, int32_t  height);
		virtual void PatchPipelineState(Renderer::Pipeline* refToPS, uint32_t definesFlags) override;
		virtual  Renderer::InputLayoutDescription GetInputLayoutDescription(size_t definesFlags) override;
		virtual Renderer::PipelineFactoryDescription GetFactoryDescription() override;
		virtual const char* GetShaderName() override;
	};


	struct DrawCall {
		DrawCall(const MeshHashData& particles, const ParticlesDrawData& data) : particles(particles), data(data) {};
		MeshHashData particles;
		ParticlesDrawData data;
	};


public:
	explicit ParticlesRenderPass(BaseRenderSystem& renderSystem);

	virtual void Init(const char* dirr) override;
	

	void Draw(const ParticlesMesh& particles, const ParticlesDrawData& data);
	
	void Resize() override {};
	void PreRender() override{};
	
	void Render();

	void Clear();
	void PostRender() override{Clear();};

	~ParticlesRenderPass() override;

private:

	void Destroy();

	Renderer::Viewport vp;
	DynamicMeshBuffer<ParticleVertex> particlesBuffer;



	std::vector<DrawCall> drawCalls;

};

