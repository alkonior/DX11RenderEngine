#pragma once
#include "GraphicsBase.h"
#include "UPRendererFactory.h"
#include "UPRendererUtils.h"
#include "UPConstBuffers.h"
#include "Renderers\BaseRenderer.h"
#include "ResourceManagers/TexturesManager.h"


struct UPDrawData {
	//UPModelData model;
	Transform position;
	float2 texOffset;
	float4 light;

	bool dynamic;

	bool dynamicLight = false;
	ImageUpdate* lightUpdate = nullptr;

	uint64_t flags;
};

class UPRenderer : public BaseRenderer {
	struct UPRendererProvider;

	UPRendererFactory* factory = nullptr;
	UPRendererProvider* provider = nullptr;

	struct UPRendererProvider : public Renderer::IStateProvider {
		int32_t width, height;
		UPRendererProvider(int32_t width, int32_t  height);
		virtual void PatchPipelineState(Renderer::PipelineState* refToPS, size_t definesFlags) override;
		virtual  Renderer::InputLayoutDescription GetInputLayoutDescription(size_t definesFlags) override;
		virtual const char* GetShaderName() override;
	};
	

	struct DrawCall {
		DrawCall(MeshHashData model, TexturesManager::TextureCache texture, UPDrawData data);
		DrawCall(MeshHashData model, TexturesManager::TextureCache texture, TexturesManager::TextureCache lightMap, UPDrawData data);

		MeshHashData model; TexturesManager::TextureCache texture; UPDrawData data;
		TexturesManager::TextureCache lightMap;
	};


public:

	UPRenderer();

	virtual void Init(void* shaderData, size_t dataSize) override;

	MeshHashData Register(UPModelData model, bool dynamic);
	void Draw(MeshHashData model, TexturesManager::TextureCache texture, UPDrawData data);
	void Draw(MeshHashData model, TexturesManager::TextureCache texture, TexturesManager::TextureCache lightMap, UPDrawData data);
	void DrawSet(MeshHashData model, UPModelData newModel, TexturesManager::TextureCache texture, UPDrawData data);

	void Render(GraphicsBase& gfx);
	void Flush();
	virtual void Clear(GraphicsBase& gfx) override;
	virtual void Clear() override {}; 

	~UPRenderer() override;

private:

	void Destroy();


	DynamicMeshBuffer<UPVertex> staticMeshes;
	DynamicMeshBuffer<UPVertex> dynamicMeshes;

	//void ResizeBigVertexBuffer(size_t newVertexBuffSize);
	//void ResizeBigIndexBuffer(size_t newIndexBuffSize);


	UPCosntBuffer dataBuffer;

	Renderer::ConstBuffer* pDataCB;

	Renderer::SamplerState sampler;
	Renderer::SamplerState lightSampler;
	std::vector<DrawCall> drawCalls;

};

