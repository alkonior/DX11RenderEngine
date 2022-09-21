#pragma once
#include "IRenderer/Utils.h"
#include "IRenderer/BlendState.h"
#include "IRenderer/DepthStencilState.h"
#include "IRenderer/RasterizerState.h"
#include "IRenderer/SamplerState.h"

namespace Renderer {
struct PipelineState {


	PixelShader* ps = nullptr;
	VertexShader* vs = nullptr;

	GeometryShader* gs = nullptr;
	ComputeShader* cs = nullptr;

	DepthStencilState* dss = nullptr;
	BlendState* bs = nullptr;
	RasterizerState* rs = nullptr; 

};


struct InputLayoutDescription {
	void* inputLayout = nullptr;
	size_t inputLayoutSize = 0;
};

struct IStateProvider {
	virtual void PatchPipelineState(PipelineState* refToPS, size_t definesFlags) = 0;
	virtual InputLayoutDescription GetInputLayoutDescription(size_t definesFlags) = 0;
	virtual const char* GetShaderName() = 0;
	virtual ~IStateProvider() = default;
};

}
