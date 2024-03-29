#pragma once
#include "CoreRenderSystem/PipelineFactory.h"
#include "TransformUtils.h"


enum BloomRendererDefines : std::uint32_t {
	BLOOMZERO = 0,
	BLOOMRED = 1,
	BLOOMTHRESHOLD = 2,
	BLOOMVERTICAL = 4,
	BLOOMHORISONTAL = 8,
	BLOOMEND = 16,

};

const Renderer::ShaderDefines BloomRendererDefines[] = {
	Renderer::ShaderDefines("RED"),
	Renderer::ShaderDefines("THRESHOLD"),
	Renderer::ShaderDefines("VERTICAL"),
	Renderer::ShaderDefines("HORISONTAL"),
	Renderer::ShaderDefines("END"),
};



class BloomRendererFactory :public Renderer::PipelineFactory {
public:
	BloomRendererFactory(Renderer::IStateProvider* provider, void* shaderData, size_t dataSize)
		:PipelineFactory(provider, (const Renderer::ShaderDefines*)BloomRendererDefines, std::size(BloomRendererDefines), shaderData, dataSize,
#ifdef _DEBUG 
(1 << 0)
#else
0
#endif
) {}
};

