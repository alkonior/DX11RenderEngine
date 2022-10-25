#pragma once
#include "BaseRenderPass.h"

template<size_t Id>
class QuadRenderPass
{
protected:
	static bool isInited;	
	static size_t renderPassesCounter;	
	static Renderer::VertexBufferBinding vertexBuffer;
	static Renderer::Buffer* indexBuffer;
	static Renderer::IRenderer* renderDevice;
	
	struct QuadRendererVertex
	{
		float2 pos;
		float2 uv;
	};
	
public:

	QuadRenderPass(Renderer::IRenderer* renderer)
	{
		QuadInit(renderer);
	};

	void QuadInit(Renderer::IRenderer* renderer)
	{
		renderPassesCounter++;
		if (isInited)
			return;

		renderDevice = renderer;
		int32_t width, height;
		renderer->GetBackbufferSize(&width, &height);


		QuadRendererVertex vertices[] =
		{
			{  float2(-1.0f, 1.0f),  float2(0.0f, 0.0f), },
			{  float2(-1.0f, -1.0f),  float2(0.0f, 1.0f), },
			{  float2(1.0f,  1.0f),  float2(1.0f, 0.0f), },
			{  float2(1.0f,  -1.0f),  float2(1.0f, 1.0f), },
			{  float2(1.0f,  -1.0f),  float2(1.0f, 1.0f), },
			{  float2(1.0f,  -1.0f),  float2(1.0f, 1.0f), },
		};
		vertexBuffer.buffersCount = 1;
		vertexBuffer.vertexBuffers = new Renderer::Buffer * [1]();
		vertexBuffer.vertexBuffers[0] = renderer->GenVertexBuffer(0, Renderer::BufferUsage::BUFFERUSAGE_NONE, sizeof(vertices));
		renderer->SetVertexBufferData(vertexBuffer.vertexBuffers[0], 0, &vertices, std::size(vertices),
			sizeof(QuadRendererVertex), sizeof(QuadRendererVertex), Renderer::SetDataOptions::SETDATAOPTIONS_NONE);
		//GFX_THROW_INFO(gfx.pDevice->CreateBuffer(&bd, &sd, &pVertexBuffer));
		vertexBuffer.vertexOffset = new (UINT)(0);
		vertexBuffer.vertexStride = new (UINT)(sizeof(QuadRendererVertex));
		// Bind vertex buffer to pipeline


		// create index buffer
		const uint16_t indices[] =
		{
			0,1,2,3,4,5
		};
		indexBuffer = renderer->GenIndexBuffer(0, Renderer::BufferUsage::BUFFERUSAGE_WRITEONLY, sizeof(indices));
		renderer->SetIndexBufferData(indexBuffer, 0, (void*)indices, sizeof(indices), Renderer::SetDataOptions::SETDATAOPTIONS_DISCARD);		
	}

	void Render()
	{
		renderDevice->ApplyMeshBuffersBinding({vertexBuffer,indexBuffer, 16});
	}
	
    ~QuadRenderPass() {
		renderPassesCounter--;
		if (renderPassesCounter == 0)
		{
			renderDevice->AddDisposeVertexBuffer(vertexBuffer.vertexBuffers[0]);
			delete vertexBuffer.vertexOffset;
			delete vertexBuffer.vertexStride;
			delete vertexBuffer.vertexBuffers;
			renderDevice->AddDisposeIndexBuffer(indexBuffer);
		}
	};

private:
	
};
