#pragma once
#include "GraphicsBase.h"
#include "ModelRendererFactory.h"
#include "ResourceManagers/TexturesManager.h"
#include "ResourceManagers/ModelsManager.h"
#include "ModelConstBuffers.h"

struct LerpModelDrawData {
	Transform position;
	bool isSingle;

	float alpha;
	size_t currentFrame;
	size_t nextFrame;
	float4 color;

	uint64_t flags;

};

class ModelRenderer {
	struct ModelRendererProvider;

	Renderer::IRenderer* renderer;
	ModelRendererFactory* factory = nullptr;
	ModelRendererProvider* provider = nullptr;

	struct ModelRendererProvider : public Renderer::IStateProvider {
		int32_t width, height;
		ModelRendererProvider(int32_t width, int32_t  height);
		virtual void PatchPipelineState(Renderer::PipelineState* refToPS, size_t definesFlags) override;
		virtual  Renderer::InputLayoutDescription GetInputLayoutDescription(size_t definesFlags) override;
		virtual ~ModelRendererProvider() override;
	};
	
	struct DrawCall {
		DrawCall(ModelsManager::ModelCache model, TexturesManager::TextureCache texture, Transform position, size_t flags);

		ModelsManager::ModelCache model; TexturesManager::TextureCache texture; Transform position; size_t flags;
	};	
	
	struct DrawLerpCall {
		DrawLerpCall(ModelsManager::ModelCache model, TexturesManager::TextureCache texture, const LerpModelDrawData&);

		ModelsManager::ModelCache model; TexturesManager::TextureCache texture; 
		LerpModelDrawData data;
	};


public:

	ModelRenderer();

	void Init(void* shaderData, size_t dataSize);
	void Init(LPCWSTR dirr);

	void Draw(ModelsManager::ModelCache model, TexturesManager::TextureCache texture, Transform position, size_t flags);
	void DrawLerp(ModelsManager::ModelCache model, TexturesManager::TextureCache texture, const LerpModelDrawData&);

	void Render(const GraphicsBase& gfx);
	void Clear();
	~ModelRenderer();
private:

	Renderer::Viewport vp;

	void Destroy();

	Shader3DTranformCosntBuffer transformBuffer;
	Shader3DDataCosntBuffer dataBuffer;

	Renderer::ConstBuffer* pTransformCB;
	Renderer::ConstBuffer* pDataCB;

	Renderer::SamplerState sampler;
	std::vector<DrawCall> drawCalls;
	std::vector<DrawLerpCall> drawLerpCalls;

};

