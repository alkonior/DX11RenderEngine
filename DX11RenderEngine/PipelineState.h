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

	DepthStencilState dss;
	BlendState bs;
	RasterizerState rs;
	Color bf;
	Viewport vp;

};


struct InputLayoutDescription {
	void* inputLayout;
	size_t inputLayoutSize;
};

struct IStateProvider {
	virtual void PatchPipelineState(PipelineState* refToPS, size_t definesFlags) = 0;
	virtual InputLayoutDescription GetInputLayoutDescription(size_t definesFlags) = 0;
	virtual ~IStateProvider();
};

}
