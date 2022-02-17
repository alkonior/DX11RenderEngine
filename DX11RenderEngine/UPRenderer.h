#pragma once
#include "UPRendererFactory.h"
#include "TexturesManager.h"

struct UPHashData {
	int vertexOffset = 0;
	int indexOffset = 0;
	int numElem = 0;
	Renderer::PrimitiveType pt;
};

struct UPDrawData {
	//UPModelData model;
	Transform position;
	float2 texOffset;
	float4 light;
	uint64_t flags;
};

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
		DrawCall(UPHashData model, TexturesManager::TextureCache texture, UPDrawData data);

		UPHashData model; TexturesManager::TextureCache texture; UPDrawData data;
	};


public:

	UPRenderer(Renderer::IRenderer* renderer);

	void Init(void* shaderData, size_t dataSize);
	void Init(LPCWSTR dirr);

	UPHashData Register(UPModelData model);
	void Draw(UPHashData model, TexturesManager::TextureCache texture, UPDrawData data);
	void DrawSet(UPHashData model, UPModelData newModel, TexturesManager::TextureCache texture, UPDrawData data);

	void Render(const GraphicsBase& gfx);
	void Flush();
	void Clear();

	~UPRenderer();

private:

	void Destroy();


	size_t bigVertexBuffSize = 0;
	size_t bigIndexBuffSize = 0;
	size_t bigVertexBuffCapacity = 2000;
	size_t bigIndexBuffCapacity = 2000;
	Renderer::VertexBufferBinding bigVertexBuffer;
	Renderer::Buffer* bigIndexBuffer = nullptr;
	void ResizeBigVertexBuffer(size_t newVertexBuffSize);
	void ResizeBigIndexBuffer(size_t newIndexBuffSize);
	std::vector<UPVertex> cpuVerticies;
	std::vector<std::uint32_t> cpuIndexes;

	UPTranformCosntBuffer transformBuffer;
	UPDataCosntBuffer dataBuffer;

	Renderer::ConstBuffer* pTransformCB;
	Renderer::ConstBuffer* pDataCB;

	Renderer::SamplerState sampler;
	std::vector<DrawCall> drawCalls;

};

