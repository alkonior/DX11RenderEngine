#pragma once
#include "IRenderer/PipelineState/PipelineFactory.h"
#include "../../Utils/TransformUtils.h"


enum  TAARendererDefines : std::uint32_t {
	TAAZERO = 0,
	TAARED = 1,
	TAACOPY = 2,
	TAAUSEYUV = 4,

};

const Renderer::ShaderDefines TAARendererDefines[] = {
	Renderer::ShaderDefines("RED"),
	Renderer::ShaderDefines("COPY"),
	Renderer::ShaderDefines("USEYUV"),
};


class TAARendererFactory :public Renderer::PipelineFactory {
public:
	TAARendererFactory(Renderer::IStateProvider* provider, void* shaderData, size_t dataSize) 
		:PipelineFactory(provider, (const Renderer::ShaderDefines*)TAARendererDefines, std::size(TAARendererDefines), shaderData, dataSize,
#ifdef _DEBUG 
		D3DCOMPILE_DEBUG
#else
0
#endif
) {}
};

