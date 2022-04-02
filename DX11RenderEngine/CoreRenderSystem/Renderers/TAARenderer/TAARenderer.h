#pragma 
#include "../QuadRenderer.h"
#include "TAARendererFactory.h"
#include "TAARendererConstBuffer.h"


class TAARenderer:public QuadRenderer {
	struct TAARendererProvider;
	TAARendererFactory* factory = nullptr;
	TAARendererProvider* provider = nullptr;

	Renderer::Texture* skyTexture = nullptr;

	struct TAARendererProvider : public Renderer::IStateProvider {
		int32_t width, height;
		TAARendererProvider(int32_t width, int32_t  height);
		virtual void PatchPipelineState(Renderer::PipelineState* refToPS, size_t definesFlags) override;
		virtual  Renderer::InputLayoutDescription GetInputLayoutDescription(size_t definesFlags) override;
		virtual const char* GetShaderName() override;
	};


public:


	TAARenderer();

	virtual void Init(void* shaderData, size_t dataSize) override;

	void Render(GraphicsBase& gfx);
	
	virtual void Clear() override {}; 
	virtual void Clear(GraphicsBase&) override; 

	
	size_t HaltonIndex;
	std::vector<float2> HaltonSequence;
	void UpdateHaltonSequence();

	~TAARenderer() override;
private:
	Renderer::ConstBuffer* constBuffer;
	TAACosntBuffer localBuffer;

	bool markNoHistoryPixels = false;
	bool allowDepthThreshold = true;
	bool allowBicubicFilter = true;
	bool allowVarianceClipping = true;
	bool allowYCoCg = true;
	bool allowNeighbourhoodSampling = true;
	bool allowLongestVelocityVector = true;

	Renderer::Texture* TAAHistory;
	
	void RenderIMGUI(GraphicsBase& gfx);
};

