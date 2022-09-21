#pragma once
#include "BeamRendererFactory.h"
#include "BeamConstBuffer.h"
#include "CoreRenderSystem/GraphicsBase.h"



struct BeamDrawData {
	uint64_t flags;
};

class BeamRenderer {
	struct BeamRendererProvider;

	BeamRendererFactory* factory = nullptr;
	BeamRendererProvider* provider = nullptr;

	struct BeamRendererProvider : public Renderer::IStateProvider {
		int32_t width, height;
		BeamRendererProvider(int32_t width, int32_t  height);
		virtual void PatchPipelineState(Renderer::PipelineState* refToPS, size_t definesFlags) override;
		virtual  Renderer::InputLayoutDescription GetInputLayoutDescription(size_t definesFlags) override;
		virtual const char* GetShaderName() override;
	};


	struct DrawCall {
		DrawCall(const BeamDrawData& data) :  data(data) {};
		BeamDrawData data;
	};


public:

	BeamRenderer();

	void Init(void* shaderData, size_t dataSize);
	void Init(LPCWSTR dirr);

	void Draw(const BeamDrawData& data);
	
	void Render(const GraphicsBase& gfx);
	void Clear();

	~BeamRenderer();

private:

	void Destroy();

	Renderer::Viewport vp;



	std::vector<DrawCall> drawCalls;

};

