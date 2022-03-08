#pragma once
#include "IRenderer/PipelineState/PipelineFactory.h"
#include "../../Utils/TransformUtils.h"


enum  EndRendererDefines : std::uint32_t {
	ENDZERO = 0,
	ENDRED = 1,
	ENDALPHA = 2,
	ENDBLOOMONLY = 4,
	ENDCOLORONLY = 8,
	ENDLIGHTONLY = 16,
	ENDALPHAONLY = 16,

};

const Renderer::ShaderDefines EndRendererDefines[] = {
	Renderer::ShaderDefines("RED"),
	Renderer::ShaderDefines("ALPHA"),
	Renderer::ShaderDefines("BLOOMONLY"),
	Renderer::ShaderDefines("COLORONLY"),
	Renderer::ShaderDefines("LIGHTONLY"),
	Renderer::ShaderDefines("ALPHAONLY"),
};


struct VertexEnd {
	float2 pos;
	float2 uv;
};

class EndRendererFactory :public Renderer::PipelineFactory {
public:
	EndRendererFactory(Renderer::IRenderer* renderer, Renderer::IStateProvider* provider, void* shaderData, size_t dataSize) 
		:PipelineFactory(renderer, provider, (const Renderer::ShaderDefines*)EndRendererDefines, std::size(EndRendererDefines), shaderData, dataSize, D3DCOMPILE_DEBUG) {}
};

