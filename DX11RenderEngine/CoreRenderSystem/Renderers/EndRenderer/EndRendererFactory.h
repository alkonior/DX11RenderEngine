#pragma once
#include "IRenderer/PipelineState/PipelineFactory.h"
#include "../../Utils/TransformUtils.h"


enum  EndRendererDefines : std::uint32_t {
	ENDZERO = 0,
	ENDRED = 1,
	ENDBLUME = 2,
	ENDALPHA = 4,

};

const Renderer::ShaderDefines EndRendererDefines[] = {
	Renderer::ShaderDefines("RED"),
	Renderer::ShaderDefines("BLUME"),
	Renderer::ShaderDefines("ALPHA"),
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

