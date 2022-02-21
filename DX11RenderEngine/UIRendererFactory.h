#pragma once
#include "PipelineFactory.h"
#include "TransformUtils.h"


enum  UIDefines : std::uint32_t {
	UIZERO = 0,
	UIRED = 1,
	UICOLORED = 2
};


struct Vertex2D {
	float2 pos;
	float2 uv;
};



class UIRendererFactory :public Renderer::PipelineFactory {
public:
	UIRendererFactory(Renderer::IRenderer* renderer, Renderer::IStateProvider* provider, void* shaderData, size_t dataSize);

};

