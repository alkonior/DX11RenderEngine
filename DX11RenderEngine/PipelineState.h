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


struct IStateProvider {
	virtual void PatchPipelineState(PipelineState* refToPS) = 0;
};

}
