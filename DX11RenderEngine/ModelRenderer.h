#pragma once
#include "ModelRendererFactory.h"
#include "TexturesManager.h"
#include "ModelsManager.h"



class ModelRenderer {
	struct ModelRendererProvider;

	Renderer::IRenderer* renderer;
	ModelRendererFactory* factory = nullptr;
	ModelRendererProvider* provider = nullptr;

	struct ModelRendererProvider : public Renderer::IStateProvider {
		int32_t width, height;
		ModelRendererProvider(int32_t width, int32_t  height);
		virtual void PatchPipelineState(Renderer::PipelineState* refToPS) override;
	};
	
	struct DrawCall {
		DrawCall(ModelsManager::ModelCache model, TexturesManager::TextureCache texture, Transform position, size_t flags);

		ModelsManager::ModelCache model; TexturesManager::TextureCache texture; Transform position; size_t flags;
	};


public:

	ModelRenderer(Renderer::IRenderer* renderer);

	void Init(void* shaderData, size_t dataSize);
	void Init(LPCWSTR dirr);

	void Draw(ModelsManager::ModelCache model, TexturesManager::TextureCache texture, Transform position, size_t flags);

	void Render(const GraphicsBase& gfx);


private:

	Shader3DCosntBuffer localBuffer;
	Renderer::ConstBuffer* constBuffer;

	Renderer::SamplerState sampler;
	std::vector<DrawCall> drawCalls;

};

