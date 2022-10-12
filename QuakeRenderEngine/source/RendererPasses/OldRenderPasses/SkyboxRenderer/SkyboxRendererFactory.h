#pragma once
#include "CoreRenderSystem/PipelineFactory.h"
#include "TransformUtils.h"




const Renderer::ShaderDefines SkyboxRendererDefines[] = {
	Renderer::ShaderDefines("RED"),
};


struct VertexSkybox {
	VertexSkybox(float3 pos) :pos(pos) {};
	float3 pos;
};



class SkyboxRendererFactory :public Renderer::PipelineFactory {
public:
	SkyboxRendererFactory(Renderer::IStateProvider* provider, void* shaderData, size_t dataSize);

};

