#pragma once
#include "IRenderer/PipelineState/PipelineFactory.h"
#include "../../Utils/TransformUtils.h"


enum  PPRendererDefines : std::uint32_t {
	PPZERO = 0,
	PPRED = 1,
	PPALPHA = 2,
	PPBLOOMONLY = 4,
	PPCOLORONLY = 8,
	PPLIGHTONLY = 16,
	PPALPHAONLY = 32,
	PPOCCLUSIONONLY = 64,
	NORMALSONLY = 128,

};

const Renderer::ShaderDefines PPRendererDefines[] = {
	Renderer::ShaderDefines("RED"),
	Renderer::ShaderDefines("ALPHA"),
	Renderer::ShaderDefines("BLOOMONLY"),
	Renderer::ShaderDefines("COLORONLY"),
	Renderer::ShaderDefines("LIGHTONLY"),
	Renderer::ShaderDefines("ALPHAONLY"),
	Renderer::ShaderDefines("OCCLUSIONONLY"),
	Renderer::ShaderDefines("NORMALSONLY"),
};

class PPRendererFactory :public Renderer::PipelineFactory {
public:
	PPRendererFactory(Renderer::IStateProvider* provider, void* shaderData, size_t dataSize) 
		:PipelineFactory(provider, (const Renderer::ShaderDefines*)PPRendererDefines, std::size(PPRendererDefines), shaderData, dataSize,
#ifdef _DEBUG 
		D3DCOMPILE_DEBUG
#else
0
#endif
) {}
};

