#pragma 
#include "../../GraphicsBase.h"
#include "../../ResourceManagers/TexturesManager.h"
#include "EndRendererFactory.h"
#include "EndRendererConstBuffer.h"


class EndRenderer {
	struct EndRendererProvider;
	Renderer::IRenderer* renderer;
	EndRendererFactory* factory = nullptr;
	EndRendererProvider* provider = nullptr;

	Renderer::Texture* skyTexture = nullptr;

	struct EndRendererProvider : public Renderer::IStateProvider {
		int32_t width, height;
		EndRendererProvider(int32_t width, int32_t  height);
		virtual void PatchPipelineState(Renderer::PipelineState* refToPS, size_t definesFlags) override;
		virtual  Renderer::InputLayoutDescription GetInputLayoutDescription(size_t definesFlags) override;
		virtual ~EndRendererProvider() override;
	};


public:


	EndRenderer(Renderer::IRenderer* renderer);

	void Init(void* shaderData, size_t dataSize);
	void Init(LPCWSTR dirr);

	void Render(GraphicsBase& gfx);

	~EndRenderer();
private:
	Renderer::Viewport vp;

	Renderer::VertexBufferBinding vertexBuffer;
	Renderer::Buffer* indexBuffer;

	Renderer::SamplerState diffuseSampler;
	Renderer::SamplerState sampler;

};

