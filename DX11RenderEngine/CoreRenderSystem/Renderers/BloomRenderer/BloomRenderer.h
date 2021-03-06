#pragma once
#include "IRenderer/IRenderer.h"
#include "CoreRenderSystem/GraphicsBase.h"
#include "BloomFactory.h"
#include "BloomConstBuffer.h"
#include "..\QuadRenderer.h"
#include "ResourceManagers/TexturesManager.h"


class BloomRenderer : public QuadRenderer {
	struct BloomRendererProvider;
	BloomRendererFactory* factory = nullptr;
	BloomRendererProvider* provider = nullptr;

	Renderer::Texture* skyTexture = nullptr;

	struct BloomRendererProvider : public Renderer::IStateProvider {
		int32_t width, height;
		BloomRendererProvider(int32_t width, int32_t  height);
		virtual void PatchPipelineState(Renderer::PipelineState* refToPS, size_t definesFlags) override;
		virtual  Renderer::InputLayoutDescription GetInputLayoutDescription(size_t definesFlags) override;
		virtual const char* GetShaderName() override;
	};


public:


	BloomRenderer();

	virtual void Init(void* shaderData, size_t dataSize) override;

	void RenderBloomMask(GraphicsBase& gfx);
	
	void Render(GraphicsBase& gfx);

	virtual void Clear() override {};
	
	~BloomRenderer() override;
private:
	int kernel;
	
	Renderer::ConstBuffer* constBuffer;
	BloomCosntBuffer localBuffer;
	

	Renderer::Texture* bloom1;
	Renderer::RenderTargetBinding  bloom1RT;
	
	Renderer::Texture* bloom2;
	Renderer::RenderTargetBinding  bloom2RT;
	
	
	Renderer::SamplerState sampler;

	void RenderIMGUI(GraphicsBase& gfx);
};

