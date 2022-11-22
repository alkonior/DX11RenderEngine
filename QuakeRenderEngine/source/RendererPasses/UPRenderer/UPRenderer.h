#pragma once
#include "UPRendererFactory.h"
#include "UPConstBuffers.h"
#include "CoreRenderSystem/RenderPasses/BaseRenderPass.h"
#include "ResourceManagers/TexturesManager.h"
#include "Utils/DrawData.h"
#include "DynamicMeshBuffer.h"



class UPRenderer : public BaseRenderPass {
	struct UPRendererProvider;

	UPRendererProvider* provider = nullptr;

	struct UPRendererProvider : public Renderer::IStateProvider {
		int32_t width, height;
		UPRendererProvider();
		virtual void PatchPipelineState(Renderer::Pipeline* refToPS, size_t definesFlags) override;
		virtual  Renderer::InputLayoutDescription GetInputLayoutDescription(size_t definesFlags) override;
		virtual const char* GetShaderName() override;
		virtual Renderer::PipelineFactoryDescription GetFactoryDescription() override;
		
	};
	

	struct DrawCall {
		DrawCall(MeshHashData model, TexturesManager::TextureCache texture, UPDrawData data);
		DrawCall(MeshHashData model, TexturesManager::TextureCache texture, TexturesManager::TextureCache lightMap, UPDrawData data);

		MeshHashData model; TexturesManager::TextureCache texture; UPDrawData data;
		TexturesManager::TextureCache lightMap;
	};


public:

	UPRenderer(BaseRenderSystem& renderSystem);

	virtual void Init(const char* dirr) override;

	MeshHashData Register(UPModelMesh model, bool dynamic);
	void Draw(MeshHashData model, TexturesManager::TextureCache texture, UPDrawData data);
	void Draw(MeshHashData model, TexturesManager::TextureCache texture, TexturesManager::TextureCache lightMap, UPDrawData data);
	void DrawSet(MeshHashData model, UPModelMesh newModel, TexturesManager::TextureCache texture, UPDrawData data);

	void Render();
	void Flush();
	virtual void PostRender() override;

	~UPRenderer() override;

private:

	void Destroy();
public:
	void Resize() override{};
	void PreRender() override ;
private:
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

