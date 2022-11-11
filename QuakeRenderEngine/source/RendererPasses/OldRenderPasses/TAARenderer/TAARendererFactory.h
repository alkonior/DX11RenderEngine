#pragma once
#include "CoreRenderSystem//PipelineFactory.h"
#include "TransformUtils.h"





class TAARendererFactory :public Renderer::PipelineFactory {
public:
	TAARendererFactory(Renderer::IStateProvider* provider, void* shaderData, size_t dataSize) 
		:PipelineFactory(provider, (const Renderer::ShaderDefines*)TAARendererDefines, std::size(TAARendererDefines), shaderData, dataSize,
#ifdef _DEBUG 
		1
#else
0
#endif
) {}
};

