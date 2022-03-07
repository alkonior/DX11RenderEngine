#pragma once
#include "IRenderer.h"
#include "GraphicsBase.h"
#include "BloomFactory.h"
#include "BloomConstBuffer.h"
#include "ResourceManagers/TexturesManager.h"


class BloomRenderer {
	struct BloomRendererProvider;
	Renderer::IRenderer* renderer;
	BloomRendererFactory* factory = nullptr;
	BloomRendererProvider* provider = nullptr;

	Renderer::Texture* skyTexture = nullptr;

	struct BloomRendererProvider : public Renderer::IStateProvider {
		int32_t width, height;
		BloomRendererProvider(int32_t width, int32_t  height);
		virtual void PatchPipelineState(Renderer::PipelineState* refToPS, size_t definesFlags) override;
		virtual  Renderer::InputLayoutDescription GetInputLayoutDescription(size_t definesFlags) override;
		virtual ~BloomRendererProvider() override;
	};


public:


	BloomRenderer(Renderer::IRenderer* renderer);

	void Init(void* shaderData, size_t dataSize);
	void Init(LPCWSTR dirr);

	void Render(GraphicsBase& gfx);

	~BloomRenderer();
private:


	Renderer::SamplerState sampler;

};

