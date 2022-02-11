#pragma once
#include "PipelineFactory.h"
#include "Transform.h"


enum class UIDefines {
	ZERO = 0,
	RED = 1,
	COLORED = 2
};

struct Vertex2D {
	DirectX::SimpleMath::Vector2 pos;
	DirectX::SimpleMath::Vector2 uv;
};

struct Shader2DCosntBuffer {
	DirectX::SimpleMath::Matrix transform;
	DirectX::SimpleMath::Vector2 uvShift;
	DirectX::SimpleMath::Vector2 uvScale;
	float4 color;
};


class UIRendererFactory :public Renderer::PipelineFactory {
public:
	UIRendererFactory(Renderer::IRenderer* renderer, Renderer::IStateProvider* provider, void* shaderData, size_t dataSize);

};

