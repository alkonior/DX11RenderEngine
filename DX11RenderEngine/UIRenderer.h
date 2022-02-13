#pragma once
#include "Renderer.h"
#include "UIRendererFactory.h"
#include "TexturesManager.h"



class UIRenderer {
	struct UIRendererProvider;
	Renderer::IRenderer* renderer;
	UIRendererFactory* factory = nullptr;
	UIRendererProvider* provider = nullptr;


	struct UIRendererProvider : public Renderer::IStateProvider {
		int32_t width, height;
		UIRendererProvider(int32_t width, int32_t  height);
		virtual void PatchPipelineState(Renderer::PipelineState* refToPS, size_t definesFlags) override;
		virtual  Renderer::InputLayoutDescription GetInputLayoutDescription(size_t definesFlags) override;
		virtual ~UIRendererProvider() override;
	};


	struct DrawCall {
		DrawCall(TexturesManager::TextureCache texture, size_t top, size_t left, size_t texW, size_t texH, size_t x, size_t y, size_t width, size_t height, uint32_t flag);
		DrawCall(TexturesManager::TextureCache texture, size_t x, size_t y, size_t width, size_t height, uint32_t flag);
		DrawCall(float4 color, size_t x, size_t y, size_t width, size_t height, uint32_t flag);

		dx::SimpleMath::Matrix getTransform(size_t screenW, size_t screenH);
		dx::SimpleMath::Vector2 getUVShift();
		dx::SimpleMath::Vector2 getUVScale();

		size_t x; size_t y; size_t width; size_t height; size_t top; size_t left; size_t texW; size_t texH;
		float4 color = {0,0,0,0};
		TexturesManager::TextureCache texture;
		uint32_t flag;
	};

public:


	UIRenderer(Renderer::IRenderer* renderer);

	void Init(void* shaderData, size_t dataSize);
	void Init(LPCWSTR dirr);

	void Destroy();

	void Draw(TexturesManager::TextureCache texture, size_t top, size_t left, size_t texW, size_t texH, size_t x, size_t y, size_t width, size_t height, uint32_t flag);
	void Draw(TexturesManager::TextureCache texture, size_t x, size_t y, size_t width, size_t height, uint32_t flag);
	void Draw(float4 color,  size_t x, size_t y, size_t width, size_t height, uint32_t flag);

	void Render();
	void Clear();
	
	~UIRenderer();
private:
	Renderer::VertexBufferBinding vertexBuffer;
	Renderer::Buffer* indexBuffer;
	Shader2DCosntBuffer localBuffer;
	Renderer::ConstBuffer* constBuffer;

	Renderer::SamplerState sampler;
	std::vector<DrawCall> drawCalls;
};

