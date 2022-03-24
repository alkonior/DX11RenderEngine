#pragma once
#include "GraphicsBase.h"
#include "ModelRendererFactory.h"
#include "ResourceManagers/TexturesManager.h"
#include "ResourceManagers/ModelsManager.h"
#include "ModelConstBuffers.h"
#include "..\..\..\..\..\..\ref_dx11\external\debug_console.h"
#include "Renderers\BaseRenderer.h"

struct LerpModelDrawData {
	Transform oldPosition;
	Transform newPosition;
	bool isSingle;
	bool isGun;

	float alpha;
	float oldAlpha;
	size_t currentFrame;
	size_t nextFrame;
	float4 color;

	uint64_t flags;

};

class ModelRenderer :public BaseRenderer {
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

	virtual void Init(void* shaderData, size_t dataSize) override;

	void Draw(ModelsManager::ModelCache model, TexturesManager::TextureCache texture, Transform position, size_t flags);
	void DrawLerp(ModelsManager::ModelCache model, TexturesManager::TextureCache texture, const LerpModelDrawData&);

	void Render(GraphicsBase& gfx);
	
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

