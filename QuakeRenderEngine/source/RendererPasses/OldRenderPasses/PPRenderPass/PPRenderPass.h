#pragma 
#include "CoreRenderSystem/RenderPasses/BaseRenderPass.h"
#include "PPRendererFactory.h"
#include "PPConstBuffer.h"


class PPRenderer : public BaseRenderPass {
	struct PPRendererProvider;
	PPRendererFactory* factory = nullptr;
	PPRendererProvider* provider = nullptr;

	Renderer::Texture* skyTexture = nullptr;

	struct PPRendererProvider : public Renderer::IStateProvider {
		int32_t width, height;
		PPRendererProvider(int32_t width, int32_t  height);
		virtual void PatchPipelineState(Renderer::PipelineState* refToPS, size_t definesFlags) override;
		virtual  Renderer::InputLayoutDescription GetInputLayoutDescription(size_t definesFlags) override;
		virtual const char* GetShaderName() override;
	};


public:


	PPRenderer();

	void Init(LPCWSTR dirr);

	void Render();
	
	virtual void Clear() override {}; 

	~PPRenderer() override;
private:

	Renderer::SamplerState diffuseSampler;
	Renderer::SamplerState sampler;

	PPCosntBuffer localData;
	Renderer::ConstBuffer* pConstBuffer;
	//Settings
	bool bloomOnly;
	bool colorOnly;
	bool lightOnly;
	bool alphaOnly;
	bool blureOnly;
	bool occlusionOnly;
	bool normalsOnly;

	
	void RenderIMGUI(GraphicsBase& gfx);
public:
	void Resize() override;
	void Init(const char* dirr) override;
	void PreRender() override;
	void PostRender() override;
};

