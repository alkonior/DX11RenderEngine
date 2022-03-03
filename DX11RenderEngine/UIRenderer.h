#pragma once
#include "IRenderer.h"
#include "UIRendererFactory.h"
#include "UIConstBuffers.h"
#include "TexturesManager.h"

struct UIDrawData {

	int x; int y;
	int width; int height; 
	int top; int left; int texW; int texH;
	float4 color = { 0,0,0,0 };
	uint64_t flag;
};

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
		DrawCall(TexturesManager::TextureCache texture, const UIDrawData& data);
		DrawCall(const UIDrawData& data);

		matrix getTransform(size_t screenW, size_t screenH);
		float2 getUVShift();
		float2 getUVScale();

		TexturesManager::TextureCache texture;
		UIDrawData data;
	};

public:


	UIRenderer(Renderer::IRenderer* renderer);

	void Init(void* shaderData, size_t dataSize);
	void Init(LPCWSTR dirr);

	void Destroy();

	void Draw(TexturesManager::TextureCache texture, const UIDrawData& data);
	void Draw(const UIDrawData& data);

	void Render();
	void Clear();
	
	~UIRenderer();
private:
	Renderer::Viewport vp;

	Renderer::VertexBufferBinding vertexBuffer;
	Renderer::Buffer* indexBuffer;
	Shader2DCosntBuffer localBuffer;
	Renderer::ConstBuffer* constBuffer;

	Renderer::SamplerState sampler;
	std::vector<DrawCall> drawCalls;
};

