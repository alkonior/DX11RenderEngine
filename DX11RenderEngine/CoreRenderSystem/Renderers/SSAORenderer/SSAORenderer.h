#pragma 
#include "../QuadRenderer.h"
#include "SSAORendererFactory.h"
#include "SSAORendererConstBuffer.h"


class SSAORenderer:public QuadRenderer {
	struct SSAORendererProvider;
	SSAORendererFactory* factory = nullptr;
	SSAORendererProvider* provider = nullptr;

	Renderer::Texture* skyTexture = nullptr;

	struct SSAORendererProvider : public Renderer::IStateProvider {
		int32_t width, height;
		SSAORendererProvider(int32_t width, int32_t  height);
		virtual void PatchPipelineState(Renderer::PipelineState* refToPS, size_t definesFlags) override;
		virtual  Renderer::InputLayoutDescription GetInputLayoutDescription(size_t definesFlags) override;
		virtual const char* GetShaderName() override;
	};


public:


	SSAORenderer();

	virtual void Init(void* shaderData, size_t dataSize) override;

	void Render(GraphicsBase& gfx);
	
	virtual void Clear() override {}; 
	virtual void Clear(GraphicsBase&) override; 
	

	~SSAORenderer() override;
private:
	
	Renderer::ConstBuffer* constBuffer;
	SSAOCosntBuffer localBuffer;
	
	Renderer::Texture* randVect;
	
	void RenderIMGUI(GraphicsBase& gfx);
};

