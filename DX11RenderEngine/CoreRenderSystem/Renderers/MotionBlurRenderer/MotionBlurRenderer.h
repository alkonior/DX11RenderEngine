#pragma 
#include "../QuadRenderer.h"
#include "MotionBlurConstBuffer.h"
#include "MotionBlurRendererFactory.h"


class MotionBlurRenderer: QuadRenderer {
	struct MotionBlurRendererProvider;
	Renderer::IRenderer* renderer;
	MotionBlurRendererFactory* factory = nullptr;
	MotionBlurRendererProvider* provider = nullptr;

	Renderer::Texture* skyTexture = nullptr;

	struct MotionBlurRendererProvider : public Renderer::IStateProvider {
		int32_t width, height;
		MotionBlurRendererProvider(int32_t width, int32_t  height);
		virtual void PatchPipelineState(Renderer::PipelineState* refToPS, size_t definesFlags) override;
		virtual  Renderer::InputLayoutDescription GetInputLayoutDescription(size_t definesFlags) override;
		virtual const char* GetShaderName() override;
	};


public:


	MotionBlurRenderer();

	void Init(void* shaderData, size_t dataSize);
	void Init(LPCWSTR dirr);

	void Render(GraphicsBase& gfx);

	~MotionBlurRenderer();
private:
	Renderer::ConstBuffer* constBuffer;
	MotionBlurCosntBuffer localBuffer;


	Renderer::Texture* buffColor;
	Renderer::Texture* buffLight;
	Renderer::RenderTargetBinding buffColorRT;
	Renderer::RenderTargetBinding buffLightRT;
	
	void RenderIMGUI(GraphicsBase& gfx);
};

