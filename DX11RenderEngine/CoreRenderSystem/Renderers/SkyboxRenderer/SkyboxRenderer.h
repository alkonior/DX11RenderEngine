#pragma once
#include "IRenderer/IRenderer.h"
#include "CoreRenderSystem\GraphicsBase.h"
#include "SkyboxRendererFactory.h"
#include "SkyboxConstBuffers.h"
#include "..\BaseRenderer.h"


class SkyboxRenderer :public BaseRenderer {
	struct SkyboxRendererProvider;
	SkyboxRendererFactory* factory = nullptr;
	SkyboxRendererProvider* provider = nullptr;

	Renderer::Texture* skyTexture = nullptr;

	struct SkyboxRendererProvider : public Renderer::IStateProvider {
		int32_t width, height;
		SkyboxRendererProvider(int32_t width, int32_t  height);
		virtual void PatchPipelineState(Renderer::PipelineState* refToPS, size_t definesFlags) override;
		virtual  Renderer::InputLayoutDescription GetInputLayoutDescription(size_t definesFlags) override;
		virtual const char* GetShaderName() override;
	};


public:


	SkyboxRenderer();

	virtual void Init(void* shaderData, size_t dataSize) override;
	
	void UpdateSkybox(const TextureData& sky, int side);

	void Destroy();
	void Render(GraphicsBase& gfx);
	
	virtual void Clear() override;
	
	~SkyboxRenderer() override;
private:
	Renderer::Viewport vp;
	Renderer::VertexBufferBinding vertexBuffer;
	Renderer::Buffer* indexBuffer;

	SkyboxCosntBuffer localBuffer;
	Renderer::ConstBuffer* constBuffer;

};

