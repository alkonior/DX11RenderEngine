#pragma once
#include "IRenderer/PipelineState/PipelineFactory.h"
#include "CoreRenderSystem/Utils/TransformUtils.h"


enum MBRendererDefines : std::uint32_t {
	MBZERO = 0,
	MBRED = 1,
	MBCOPY = 2,

};

const Renderer::ShaderDefines MotionBlurRendererDefines[] = {
	Renderer::ShaderDefines("RED"),
	Renderer::ShaderDefines("COPY"),
};


class MotionBlurRendererFactory :public Renderer::PipelineFactory {
public:
	MotionBlurRendererFactory(Renderer::IStateProvider* provider, void* shaderData, size_t dataSize) 
		:PipelineFactory(provider, (const Renderer::ShaderDefines*)MotionBlurRendererDefines, std::size(MotionBlurRendererDefines), shaderData, dataSize,
#ifdef _DEBUG 
		D3DCOMPILE_DEBUG
#else
0
#endif
) {}
};

