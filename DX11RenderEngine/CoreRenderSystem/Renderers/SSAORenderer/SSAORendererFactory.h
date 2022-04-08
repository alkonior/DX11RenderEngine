#pragma once
#include "IRenderer/PipelineState/PipelineFactory.h"
#include "../../Utils/TransformUtils.h"


enum  SSAORendererDefines : std::uint32_t {
	SSAOZERO = 0,
	SSAORED = 1,
	SSAOBLUR = 2,

};

const Renderer::ShaderDefines SSAORendererDefines[] = {
	Renderer::ShaderDefines("RED"),
	Renderer::ShaderDefines("BLUR"),
};


class SSAORendererFactory :public Renderer::PipelineFactory {
public:
	SSAORendererFactory(Renderer::IStateProvider* provider, void* shaderData, size_t dataSize) 
		:PipelineFactory(provider, (const Renderer::ShaderDefines*)SSAORendererDefines, std::size(SSAORendererDefines), shaderData, dataSize,
#ifdef _DEBUG 
		D3DCOMPILE_DEBUG
#else
0
#endif
) {}
};

