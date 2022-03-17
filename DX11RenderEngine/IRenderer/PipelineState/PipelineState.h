#pragma once
#include "pch.h"
#include "Utils.h"
#include "BlendState.h"
#include "DepthStencilState.h"
#include "RasterizerState.h"
#include "SamplerState.h"

namespace Renderer {
struct PipelineState {


	PixelShader* ps;
	VertexShader* vs;

	GeometryShader* gs;
	ComputeShader* cs;

	DepthStencilState* dss;
	BlendState* bs;
	RasterizerState* rs;

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
