#pragma 
#include "../QuadRenderer.h"
#include "FXAARendererFactory.h"
#include "FXAARendererConstBuffer.h"


class FXAARenderer: QuadRenderer {
	struct FXAARendererProvider;
	Renderer::IRenderer* renderer;
	FXAARendererFactory* factory = nullptr;
	FXAARendererProvider* provider = nullptr;

	Renderer::Texture* skyTexture = nullptr;

	struct FXAARendererProvider : public Renderer::IStateProvider {
		int32_t width, height;
		FXAARendererProvider(int32_t width, int32_t  height);
		virtual void PatchPipelineState(Renderer::PipelineState* refToPS, size_t definesFlags) override;
		virtual  Renderer::InputLayoutDescription GetInputLayoutDescription(size_t definesFlags) override;
		virtual const char* GetShaderName() override;
	};


public:


	FXAARenderer();

	void Init(void* shaderData, size_t dataSize);
	void Init(LPCWSTR dirr);

	void Render(GraphicsBase& gfx);

	~FXAARenderer();
private:
	Renderer::ConstBuffer* constBuffer;
	FXAACosntBuffer localBuffer;


	Renderer::Texture* preFXAA;
	Renderer::RenderTargetBinding preFXAART;
	
	void RenderIMGUI(GraphicsBase& gfx);
};

