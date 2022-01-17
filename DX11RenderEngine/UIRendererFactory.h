#pragma once
#include "PipelineFactory.h"
#include "SimpleMath.h"


extern Renderer::ShaderDefines UIRendererDefines[ ];


struct Vertex2D {
	DirectX::SimpleMath::Vector2 pos;
	DirectX::SimpleMath::Vector2 uv;
};

extern D3D11_INPUT_ELEMENT_DESC UIRendererInputLayout[ ];
struct VertexShaderCosntBuffer {
	DirectX::SimpleMath::Matrix transform;
	DirectX::SimpleMath::Vector2 uvShift;
	DirectX::SimpleMath::Vector2 uvScale;
};

class UIRendererFactory:public Renderer::PipelineFactory {
public:
	UIRendererFactory(Renderer::IRenderer* renderer, Renderer::IStateProvider* provider, void* shaderData, size_t dataSize);


};

