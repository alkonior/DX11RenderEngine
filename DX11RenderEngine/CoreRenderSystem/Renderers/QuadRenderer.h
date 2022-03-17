#pragma once
#include "../GraphicsBase.h"
#include "IRenderer\IRenderer.h"


class QuadRenderer
{

	
	struct QuadRendererVertex
	{
		float2 pos;
		float2 uv;
	};
	
	Renderer::IRenderer* renderer;
public:

	QuadRenderer();
    void Init();
    void Render();
	Renderer::Viewport vp;
	
    ~QuadRenderer();

private:
    	
    Renderer::VertexBufferBinding vertexBuffer;
    Renderer::Buffer* indexBuffer;
	
};
