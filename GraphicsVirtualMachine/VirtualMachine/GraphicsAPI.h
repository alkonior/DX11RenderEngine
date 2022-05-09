#pragma once
#include "BaseStructures.h"
#include "IRenderClasses.h"
namespace GVM {

class GraphicsAPI {
	
public:
    
	GraphicsAPI(const RenderDeviceInitParams& initParams, bool debugMode = true) {}
	GraphicsAPI(const GraphicsAPI&) = delete;
	GraphicsAPI(const GraphicsAPI&&) = delete;
	~GraphicsAPI() = default;

	/* Presentation */

	void Present();

	/* Drawing */

	void ClearRenderTarget(const IRenderTarget** renderTargets, int32_t numRenderTargets, FColor color);
	void Clear(float depth, int8_t stencil);
	void ResetBackbuffer(const IRenderDeviceInitParams* initParams);

#pragma region DrawDispatch

	void DrawIndexedPrimitives(
		int32_t baseVertex,
		int32_t minVertexIndex,
		int32_t numVertices,
		int32_t startIndex,
		int32_t primitiveCount
	);
	
	void DrawInstancedPrimitives(
		int32_t baseVertex,
		int32_t minVertexIndex,
		int32_t numVertices,
		int32_t startIndex,
		int32_t primitiveCount,
		int32_t instanceCount
	);
	
	void DrawPrimitives(
		int32_t vertexStart,
		int32_t primitiveCount
	);
	
	void Dispatch(size_t x, size_t y, size_t z = 1);
	
#pragma endregion

#pragma region CreateDestroyPipelineStates
	const IViewport*				CreateViewport(const ViewportDesc* viewportDesc);
	const IScissorRect*				CreateScissorRect(const Rect* scissor);
	const IBlendState*				CreateBlendState(const BlendDesc* blendStates);
	const IDepthStencilState*		CreateDepthStencilState(const DepthStencilStateDesc* depthStencilState);
	const IRasterizerState*			CreateRasterizerStat(const RasterizerStateDesc* rasterizerState);
	const ISamplerState*			CreateSampler(const SamplerStateDesc* sampler);
	const IShader*					CreateShader(const ShaderDesc& shaderDesc, EShaderType type);

	void AddDisposeViewport	(const IViewport* viewportDesc);
	void AddDisposeScissorRect(const IScissorRect* scissor);
	void AddDisposeBlendState(const IBlendState* blendStates);
	void AddDisposeDepthStencilState(const IDepthStencilState* depthStencilState);
	void AddDisposeRasterizerStat(const IRasterizerState* rasterizerState);
	void AddDisposeSampler(const ISamplerState*	sampler);
	void AddDisposeShader(IShader* shader);
#pragma endregion

#pragma region SetupPipeline
	void SetViewports(const IViewport** viewport, uint8_t num);
	void SetScissorRect(const IScissorRect** scissors, uint8_t num);
	void SetBlendStates(const IBlendState* blendState, FColor blendFactor, uint32_t sampleMask);
	void SetDepthStencilState(const IDepthStencilState* depthStencilState);
	void SetRasterizerState(const IRasterizerState* rasterizerState);
	void SetSamplers(const ISamplerState** sampler, uint8_t num, EShaderType shader);
	void SetPipelineState(const IPipelineState pipelineState);


	void SetupVertexBuffer(const VertexBufferBinding& bindings);
	void SetupIndexBuffer(const IIndexBufferView* indices);
	void SetupMeshBuffers(const Mesh& bindings);

	
	void SetupTextures(const IResourceView* texture, uint8_t num, EShaderType shader);
	void SetupVertexBuffer(const IVertexBuffer* bindings);
	void SetupRenderTargets(const IRenderTarget** renderTargets, int32_t numRenderTargets, IResourceView* depthStencilBuffer, bool isCompute = false);
	void SetupShader(const IShader* shader, EShaderType type);

	void SetupConstBuffers(IConstBuffer** constBuffers, size_t num);

	void ClearPipelineState();

#pragma endregion

#pragma region CreateDestroyResizeResources
	/* Resources */
	
	IResource* CreateBuffer	  (const BufferDesc& description);
	IResource* CreateTexture1D(const Texture1DDesc& description);
	IResource* CreateTexture2D(const Texture2DDesc& description);
	IResource* CreateTexture3D(const Texture3DDesc& description);
	
	IResourceView* CreateResourceView(const IResource* resource, const ResourceViewDesc& description);
	
	IVertexBuffer* CreateVertexBuffer(const VertexBufferDesc& description);
	IIndexBuffer* CreateIndexBuffer(const IndexBufferDesc& description);
	
	//IResource* ResizeBuffer   (const IResource* texture, const BufferDesc& description);
	//IResource* ResizeTexture1D(const IResource* texture, const Texture1DDesc& description);
	//IResource* ResizeTexture2D(const IResource* texture, const Texture2DDesc& description);
	//IResource* ResizeTexture3D(const IResource* texture, const Texture3DDesc& description);
	

	void AddDisposeResource(const IResource* texture);
	//void AddDisposeConstBuffer(const IConstBuffer* constBuffers);
	//void AddDisposeVertexBuffer(const IVertexBuffer* buffer);
	//void AddDisposeIndexBuffer(const IIndexBuffer* buffer);
	
#pragma endregion

#pragma region SetResourcesData

	void SetResourceData(
		const IResource*	resource,
		uint16_t			dstSubresource,
		const UBox			rect,
		const void*			pSrcData,
		int32_t				srcRowPitch,
		int32_t				srcDepthPitch
	);
	
	void SetVertexBufferData(
		const IVertexBuffer*   vertexBuffer,
		const void*			   pSrcData,
		uint32_t			   dataLength,
		int32_t				   srcRowPitch,
		int32_t				   srcDepthPitch
		);
	
	void SetIndexBufferData(
		const IIndexBuffer* buffer,
		const void*			   pSrcData,
		uint32_t			   dataLength,
		int32_t				   srcRowPitch,
		int32_t				   srcDepthPitch
	);

	void SetConstBufferData(
		IConstBuffer* constBuffer,
		void* data);

#pragma endregion



	//void Flush();

	void BeginEvent(const char* name);
	void EndEvent();
	void SetMarker(const char* name);
    
};
}