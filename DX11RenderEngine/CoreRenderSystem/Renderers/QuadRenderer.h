#pragma once
#include "BaseRenderer.h"

class QuadRenderer : public BaseRenderer
{
	struct QuadRendererVertex
	{
		float2 pos;
		float2 uv;
	};
	
public:

	QuadRenderer(const char*);
    void Init();
    void Render();
	Renderer::Viewport vp;
	
    ~QuadRenderer() override;

private:
    	
    Renderer::VertexBufferBinding vertexBuffer;
    Renderer::Buffer* indexBuffer;
	
};
