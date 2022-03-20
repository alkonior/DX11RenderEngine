#pragma 
#include "../../GraphicsBase.h"
#include "../../ResourceManagers/TexturesManager.h"
#include "PPRendererFactory.h"
#include "PPConstBuffer.h"
#include "..\QuadRenderer.h"


class PPRenderer : public QuadRenderer {
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

	virtual void Init(void* shaderData, size_t dataSize) override;
	void Init(LPCWSTR dirr);

	void Render(GraphicsBase& gfx);
	
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

	
	void RenderIMGUI(GraphicsBase& gfx);
};

