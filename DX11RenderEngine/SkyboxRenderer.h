﻿#pragma once
#include "Renderer.h"
#include "SkyboxRendererFactory.h"
#include "SkyboxConstBuffers.h"
#include "TexturesManager.h"


class SkyboxRenderer {
	struct SkyboxRendererProvider;
	Renderer::IRenderer* renderer;
	SkyboxRendererFactory* factory = nullptr;
	SkyboxRendererProvider* provider = nullptr;

	Renderer::Texture* skyTexture = nullptr;

	struct SkyboxRendererProvider : public Renderer::IStateProvider {
		int32_t width, height;
		SkyboxRendererProvider(int32_t width, int32_t  height);
		virtual void PatchPipelineState(Renderer::PipelineState* refToPS, size_t definesFlags) override;
		virtual  Renderer::InputLayoutDescription GetInputLayoutDescription(size_t definesFlags) override;
		virtual ~SkyboxRendererProvider() override;
	};


public:


	SkyboxRenderer(Renderer::IRenderer* renderer);

	void Init(void* shaderData, size_t dataSize);
	void Init(LPCWSTR dirr);
	void UpdateSkybox(const TextureData& sky, int side);

	void Destroy();
	void Render(GraphicsBase& gfx);
	
	~SkyboxRenderer();
private:
	Renderer::VertexBufferBinding vertexBuffer;
	Renderer::Buffer* indexBuffer;

	SkyboxCosntBuffer localBuffer;
	Renderer::ConstBuffer* constBuffer;
	Renderer::SamplerState sampler;

};
