#pragma once
#include "UPRendererFactory.h"
#include "TexturesManager.h"



class UPRenderer {
	struct UPRendererProvider;

	Renderer::IRenderer* renderer;
	UPRendererFactory* factory = nullptr;
	UPRendererProvider* provider = nullptr;

	struct UPRendererProvider : public Renderer::IStateProvider {
		int32_t width, height;
		UPRendererProvider(int32_t width, int32_t  height);
		virtual void PatchPipelineState(Renderer::PipelineState* refToPS, size_t definesFlags) override;
		virtual  Renderer::InputLayoutDescription GetInputLayoutDescription(size_t definesFlags) override;
		virtual ~UPRendererProvider() override;
	};

	struct DrawCall {
		DrawCall(UPModelData model, TexturesManager::TextureCache texture, Transform position, float4 light, uint64_t flags);

		UPModelData model; TexturesManager::TextureCache texture; Transform position; float4 color; uint64_t flags;
	};


public:

	UPRenderer(Renderer::IRenderer* renderer);

	void Init(void* shaderData, size_t dataSize);
	void Init(LPCWSTR dirr);

	void Draw(UPModelData model, TexturesManager::TextureCache texture, Transform position, float4 light, size_t flags);
	
	void Render(const GraphicsBase& gfx);
	void Clear();

	~UPRenderer();

private:

	void Destroy();

	void ResizeBuffers(size_t newVertexBuffSize = 0, size_t newIndexBuffSize = 0);

	size_t vertexBuffSize = 0;
	size_t indexBuffSize = 0;
	Renderer::VertexBufferBinding vertexBuffer;
	Renderer::Buffer* indexBuffer = nullptr;


	UPTranformCosntBuffer transformBuffer;
	UPDataCosntBuffer dataBuffer;

	Renderer::ConstBuffer* pTransformCB;
	Renderer::ConstBuffer* pDataCB;

	Renderer::SamplerState sampler;
	std::vector<DrawCall> drawCalls;

};

