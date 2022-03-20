#pragma once
#include "IRenderer/PipelineState/PipelineFactory.h"
#include "Utils/TransformUtils.h"


enum  UIDefines : std::uint32_t {
	UIZERO = 0,
	UIRED = 1,
	UICOLORED = 2,
	UISCALED = 4,

	UICHAR = 32
};
const Renderer::ShaderDefines UIRendererDefines[] = {
	{"RED", "1"},
	{"COLORED", "1"},
	{"SCALED", "1"},

};


struct Vertex2D {
	float2 pos;
	float2 uv;
};



class UIRendererFactory :public Renderer::PipelineFactory {
public:
	UIRendererFactory(Renderer::IStateProvider* provider, void* shaderData, size_t dataSize);

};

