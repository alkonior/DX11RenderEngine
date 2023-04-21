#pragma once
#include "IRenderer/PipelineState/PipelineFactory.h"
#include "CoreRenderSystem/Utils/TransformUtils.h"


enum MBRendererDefines : std::uint32_t {
	MBZERO = 0,
	MBRED = 1,
	MBDYNAMIC = 2,
	MBCOPY = 4,

};

const Renderer::ShaderDefines MotionBlurRendererDefines[] = {
	Renderer::ShaderDefines("RED"),
	Renderer::ShaderDefines("DYNAMIC"),
	Renderer::ShaderDefines("COPY"),
};


class MotionBlurRendererFactory :public Renderer::PipelineFactory {
public:
	MotionBlurRendererFactory(Renderer::IStateProvider* provider, void* shaderData, size_t dataSize) 
		:PipelineFactory(provider, (const Renderer::ShaderDefines*)MotionBlurRendererDefines, std::size(MotionBlurRendererDefines), shaderData, dataSize,
#ifdef _DEBUG 
		D3DCOMPILE_DEBUG
#else
			D3DCOMPILE_DEBUG
#endif
) {}
};

