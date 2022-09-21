#pragma once
#include "CoreRenderSystem/PipelineFactory.h"
#include "TransformUtils.h"



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

