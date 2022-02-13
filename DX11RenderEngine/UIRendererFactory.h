#pragma once
#include "PipelineFactory.h"
#include "Transform.h"


enum  UIDefines : std::uint32_t {
	UIZERO = 0,
	UIRED = 1,
	UICOLORED = 2
};


struct Vertex2D {
	DirectX::SimpleMath::Vector2 pos;
	DirectX::SimpleMath::Vector2 uv;
};

struct Shader2DCosntBuffer {
	matrix transform;
	float2 uvShift;
	float2 uvScale;
	float4 color;
};


class UIRendererFactory :public Renderer::PipelineFactory {
public:
	UIRendererFactory(Renderer::IRenderer* renderer, Renderer::IStateProvider* provider, void* shaderData, size_t dataSize);

};

