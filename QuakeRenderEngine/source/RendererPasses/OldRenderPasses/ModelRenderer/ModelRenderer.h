#pragma once
#include "ModelRendererFactory.h"
#include "ModelConstBuffers.h"
#include "CoreRenderSystem/RenderPasses/BaseRenderPass.h"
#include "ResourceManagers/ModelsManager.h"
#include "ResourceManagers/TexturesManager.h"
#include "Utils/DrawData.h"


class ModelRenderer : public BaseRenderPass<void> {
	struct ModelRendererProvider;

	ModelRendererFactory* factory = nullptr;
	ModelRendererProvider* provider = nullptr;

	struct ModelRendererProvider : public Renderer::IStateProvider {
		int32_t width, height;
		ModelRendererProvider(int32_t width, int32_t  height);
		virtual void PatchPipelineState(Renderer::PipelineState* refToPS, size_t definesFlags) override;
		virtual  Renderer::InputLayoutDescription GetInputLayoutDescription(size_t definesFlags) override;
		virtual const char* GetShaderName() override;
	};
	
	struct DrawCall {
		DrawCall(ModelsManager::SavedModel model, TexturesManager::TextureCache texture, Transform position, size_t flags);

		ModelsManager::SavedModel model; TexturesManager::TextureCache texture; Transform position; size_t flags;
	};	
	
	struct DrawLerpCall {
		DrawLerpCall(ModelsManager::SavedModel model, TexturesManager::TextureCache texture, const LerpModelDrawData&);

		ModelsManager::SavedModel model; TexturesManager::TextureCache texture; 
		LerpModelDrawData data;
	};


public:

	ModelRenderer();

	//virtual void Init(void* shaderData, size_t dataSize) override;

	void Draw(ModelsManager::SavedModel model, TexturesManager::TextureCache texture, Transform position, size_t flags);
	void DrawLerp(ModelsManager::SavedModel model, TexturesManager::TextureCache texture, const LerpModelDrawData&);

	void Render() override;
	
	virtual void Clear() override;

	
	~ModelRenderer() override;
private:

	Renderer::Viewport vp;

	void Destroy();

	ModelsCosntBuffer dataBuffer;

	Renderer::ConstBuffer* pDataCB;

	std::vector<DrawCall> drawCalls;
	std::vector<DrawLerpCall> drawLerpCalls;

};

