#pragma once
#include "CoreRenderSystem/PipelineFactory.h"
#include "TransformUtils.h"


enum  SkyboxDefines : std::uint32_t {
	SKYZERO = 0,
	SKYRED = 1,
	SKYNOTHING = 2,

};
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

