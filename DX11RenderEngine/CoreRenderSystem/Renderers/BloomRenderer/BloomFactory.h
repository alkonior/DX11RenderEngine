#pragma once
#include "PipelineState/PipelineFactory.h"
#include "Utils/TransformUtils.h"


enum BloomRendererDefines : std::uint32_t {
	BLOOMZERO = 0,
	BLOOMRED = 1,

};

const Renderer::ShaderDefines BloomRendererDefines[] = {
	Renderer::ShaderDefines("RED"),
};



class BloomRendererFactory :public Renderer::PipelineFactory {
public:
	BloomRendererFactory(Renderer::IRenderer* renderer, Renderer::IStateProvider* provider, void* shaderData, size_t dataSize)
		:PipelineFactory(renderer, provider, (const Renderer::ShaderDefines*)BloomRendererDefines, std::size(BloomRendererDefines), shaderData, dataSize, D3DCOMPILE_DEBUG) {}
};

