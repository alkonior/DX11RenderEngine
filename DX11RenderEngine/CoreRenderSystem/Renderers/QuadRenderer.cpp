#include "QuadRenderer.h"

using namespace Renderer;
QuadRenderer::QuadRenderer() : renderer(IRenderer::renderer) {};
void QuadRenderer::Init()
{
    int32_t width, height;
	renderer->GetBackbufferSize(&width, &height);


	QuadRendererVertex vertices[] =
	{
		{  float2(-1.0f, 1.0f),  float2(0.0f, 0.0f), },
		{  float2(-1.0f, -1.0f),  float2(0.0f, 1.0f), },
		{  float2(1.0f,  1.0f),  float2(1.0f, 0.0f), },
		{  float2(1.0f,  -1.0f),  float2(1.0f, 1.0f), }
	};
	vertexBuffer.buffersCount = 1;
	vertexBuffer.vertexBuffers = new Buffer * [1]();
	vertexBuffer.vertexBuffers[0] = renderer->GenVertexBuffer(0, BufferUsage::BUFFERUSAGE_NONE, sizeof(vertices));
	renderer->SetVertexBufferData(vertexBuffer.vertexBuffers[0], 0, &vertices, 4,
		sizeof(QuadRendererVertex), sizeof(QuadRendererVertex), SetDataOptions::SETDATAOPTIONS_NONE);
	//GFX_THROW_INFO(gfx.pDevice->CreateBuffer(&bd, &sd, &pVertexBuffer));
	vertexBuffer.vertexOffset = new (UINT)(0);
	vertexBuffer.vertexStride = new (UINT)(sizeof(QuadRendererVertex));
	// Bind vertex buffer to pipeline


	// create index buffer
	const uint16_t indices[] =
	{
		0,1,2,3
	};
	indexBuffer = renderer->GenIndexBuffer(0, BufferUsage::BUFFERUSAGE_WRITEONLY, 12);
	renderer->SetIndexBufferData(indexBuffer, 0, (void*)indices, 12, SetDataOptions::SETDATAOPTIONS_DISCARD);


	vp.x = 0;
	vp.y = 0;
	vp.w = width;
	vp.h = height;
	vp.minDepth = 0.0f;
	vp.maxDepth = 1.0f;
	
}

void QuadRenderer::Render()
{
	renderer->ApplyVertexBufferBinding(vertexBuffer);
	renderer->ApplyIndexBufferBinding(indexBuffer, 16);
}

QuadRenderer::~QuadRenderer()
{
	delete vertexBuffer.vertexOffset;
	delete vertexBuffer.vertexStride;
	renderer->AddDisposeVertexBuffer(vertexBuffer.vertexBuffers[0]);
	delete[] vertexBuffer.vertexBuffers;
	renderer->AddDisposeIndexBuffer(indexBuffer);
}
