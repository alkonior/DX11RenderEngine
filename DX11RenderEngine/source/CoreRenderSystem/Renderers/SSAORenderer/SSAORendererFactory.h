#pragma once
#include "CoreRenderSystem//PipelineFactory.h"
#include "TransformUtils.h"


enum  SSAORendererDefines : std::uint32_t {
	SSAOZERO = 0,
	SSAORED = 1,
	SSAOBLUR = 2,
	SSAOOCCLUSION = 4,
	SSAOCOPY = 8,

};

const Renderer::ShaderDefines SSAORendererDefines[] = {
	Renderer::ShaderDefines("RED"),
	Renderer::ShaderDefines("BLUR"),
	Renderer::ShaderDefines("OCCLUSION"),
	Renderer::ShaderDefines("COPY"),
};


class SSAORendererFactory :public Renderer::PipelineFactory {
public:
	SSAORendererFactory(Renderer::IStateProvider* provider, void* shaderData, size_t dataSize) 
		:PipelineFactory(provider, (const Renderer::ShaderDefines*)SSAORendererDefines, std::size(SSAORendererDefines), shaderData, dataSize,
#ifdef _DEBUG 
		1
#else
0
#endif
) {}
};

