#pragma once
#include "PipelineFactory.h"
#include "SimpleMath.h"

struct Vertex2D {
	DirectX::SimpleMath::Vector2 pos;
	DirectX::SimpleMath::Vector2 uv;

	static const D3D11_INPUT_ELEMENT_DESC InputElements[];
};

struct Shader2DCosntBuffer {
	DirectX::SimpleMath::Matrix transform;
	DirectX::SimpleMath::Vector2 uvShift;
	DirectX::SimpleMath::Vector2 uvScale;
};

extern Renderer::ShaderDefines UIRendererDefines[ ];

class UIRendererFactory:public Renderer::PipelineFactory {
public:
	UIRendererFactory(Renderer::IRenderer* renderer, Renderer::IStateProvider* provider, void* shaderData, size_t dataSize);


};

