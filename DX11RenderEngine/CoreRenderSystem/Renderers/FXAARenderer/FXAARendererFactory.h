#pragma once
#include "IRenderer/PipelineState/PipelineFactory.h"
#include "../../Utils/TransformUtils.h"


enum  FXAARendererDefines : std::uint32_t {
	FXAAZERO = 0,
	FXAARED = 1,
	FXAASETLUMA = 2,

};

const Renderer::ShaderDefines FXAARendererDefines[] = {
	Renderer::ShaderDefines("RED"),
	Renderer::ShaderDefines("SETLUMA"),
};


class FXAARendererFactory :public Renderer::PipelineFactory {
public:
	FXAARendererFactory(Renderer::IStateProvider* provider, void* shaderData, size_t dataSize) 
		:PipelineFactory(provider, (const Renderer::ShaderDefines*)FXAARendererDefines, std::size(FXAARendererDefines), shaderData, dataSize,
#ifdef _DEBUG 
		D3DCOMPILE_DEBUG
#else
0
#endif
) {}
};

