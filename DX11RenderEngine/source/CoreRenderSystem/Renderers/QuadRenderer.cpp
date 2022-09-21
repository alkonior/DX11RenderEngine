#include "pch.h"
#include "QuadRenderer.h"

using namespace Renderer;
QuadRenderer::QuadRenderer(const char* name) : BaseRenderer(name) {};


VertexBufferBinding QuadRenderer::vertexBuffer;
Buffer* QuadRenderer::indexBuffer = nullptr;
bool QuadRenderer::isInited = false;

void QuadRenderer::Init()
{
	if (isInited)
		return;
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
	vertexBuffer.vertexBuffers = new Buffer * [1]();
	vertexBuffer.vertexBuffers[0] = renderer->GenVertexBuffer(0, BufferUsage::BUFFERUSAGE_NONE, sizeof(vertices));
	renderer->SetVertexBufferData(vertexBuffer.vertexBuffers[0], 0, &vertices, std::size(vertices),
		sizeof(QuadRendererVertex), sizeof(QuadRendererVertex), SetDataOptions::SETDATAOPTIONS_NONE);
	//GFX_THROW_INFO(gfx.pDevice->CreateBuffer(&bd, &sd, &pVertexBuffer));
	vertexBuffer.vertexOffset = new (UINT)(0);
	vertexBuffer.vertexStride = new (UINT)(sizeof(QuadRendererVertex));
	// Bind vertex buffer to pipeline


	// create index buffer
	const uint16_t indices[] =
	{
		0,1,2,3,4,5
	};
	indexBuffer = renderer->GenIndexBuffer(0, BufferUsage::BUFFERUSAGE_WRITEONLY, sizeof(indices));
	renderer->SetIndexBufferData(indexBuffer, 0, (void*)indices, sizeof(indices), SetDataOptions::SETDATAOPTIONS_DISCARD);


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
	if(!isInited)
		return;
	isInited = false;
	delete vertexBuffer.vertexOffset;
	delete vertexBuffer.vertexStride;
	renderer->AddDisposeVertexBuffer(vertexBuffer.vertexBuffers[0]);
	delete[] vertexBuffer.vertexBuffers;
	renderer->AddDisposeIndexBuffer(indexBuffer);
}
