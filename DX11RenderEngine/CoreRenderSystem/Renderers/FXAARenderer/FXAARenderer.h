#pragma 
#include "../../GraphicsBase.h"
#include "../../ResourceManagers/TexturesManager.h"
#include "FXAARendererFactory.h"
#include "FXAARendererConstBuffer.h"


class FXAARenderer {
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
		virtual ~FXAARendererProvider() override;
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
	Renderer::Viewport vp;


	Renderer::Texture* preFXAA;
	Renderer::RenderTargetBinding preFXAART;
	
	Renderer::VertexBufferBinding vertexBuffer;
	Renderer::Buffer* indexBuffer;

	Renderer::SamplerState diffuseSampler;
	Renderer::SamplerState sampler;

	//Settings
	bool bloomOnly;
	bool colorOnly;
	bool lightOnly;
	bool alphaOnly;

	
	void RenderIMGUI(GraphicsBase& gfx);
};

