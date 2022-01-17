#pragma once
#include "Renderer.h"
#include "UIRendererFactory.h"
#include "TexturesManager.h"


struct UIRendererProvider : public Renderer::IStateProvider {
	int32_t width, height;
	UIRendererProvider(int32_t width, int32_t  height);
	// Унаследовано через IStateProvider
	virtual void PatchPipelineState(Renderer::PipelineState* refToPS) override;
};

class UIRenderer {
	Renderer::IRenderer* renderer;
	UIRendererFactory* factory;

	

	struct Rectangle {
		Rectangle(TexturesManager::TextureCache texture, size_t top, size_t left, size_t texW, size_t texH, size_t x, size_t y, size_t width, size_t height, uint32_t flag);
		Rectangle(TexturesManager::TextureCache texture, size_t x, size_t y, size_t width, size_t height, uint32_t flag);

		dx::SimpleMath::Matrix getTransform(size_t screenW, size_t screenH);
		dx::SimpleMath::Vector2 getUVShift();
		dx::SimpleMath::Vector2 getUVScale();

		size_t x; size_t y; size_t width; size_t height; size_t top; size_t left; size_t texW; size_t texH;
		TexturesManager::TextureCache texture;
		uint32_t flag;
	};

public:


	UIRenderer(Renderer::IRenderer* renderer);

	void Init(void* shaderData, size_t dataSize);
	void Init(LPCWSTR dirr);

	void Draw(TexturesManager::TextureCache texture, size_t top, size_t left, size_t texW, size_t texH, size_t x, size_t y, size_t width, size_t height, uint32_t flag);
	void Draw(TexturesManager::TextureCache texture, size_t x, size_t y, size_t width, size_t height, uint32_t flag);

	void Render();
private:
	Renderer::D3D11VertexBufferBinding vertexBuffer;
	Renderer::Buffer* indexBuffer;

	VertexShaderCosntBuffer localBuffer;
	Renderer::ConstBuffer* constBuffer;
	Renderer::SamplerState sampler;


	std::vector<Rectangle> drawCalls;
};

