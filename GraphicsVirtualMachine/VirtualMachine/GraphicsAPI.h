#pragma once
#include "BaseStructures.h"
#include "VirtualMachine.h"
#include "CoreStructures\PipelineSnapshot.h"
namespace GVM {

class GraphicsApi {

#pragma region Fields
	
	PipelineSnapshot ps;
	bool wasPSUpdated = true;
	VirtualMachine graphicsMachine;

#pragma endregion 
	
public:
    
	GraphicsApi(const RenderDeviceInitParams& initParams, bool debugMode = true);
	GraphicsApi(const GraphicsApi&) = delete;
	GraphicsApi(const GraphicsApi&&) = delete;
	~GraphicsApi() = default;

	/* Presentation */

	void Present();

#pragma region Clear

	void ClearRenderTargets(const RenderTargetView** renderTargets, int32_t numRenderTargets, FColor color);
	void ClearRenderTarget(const RenderTargetView* renderTarget, FColor color);
	void Clear(const DepthStencilView* septhStencil, float depth, int8_t stencil);
	void ClearState();


#pragma endregion

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


#pragma region SetupPipeline
	void SetupViewports(const ViewportDesc viewport[], uint8_t num, uint8_t offset);
	void SetupViewport(const ViewportDesc& viewport, uint8_t slot);
	
	//void SetScissorRect(const ScissorRectDesc** scissors, uint8_t num);
	//void SetScissorRect(const ScissorRect** scissors, uint8_t num);
	void SetupCoreBlendState(const CoreBlendDesc& blendState);
	
	void SetupDepthStencilState(const DepthStencilStateDesc& depthStencilState);
	void SetupRasterizerState(const RasterizerStateDesc& rasterizerState);
	
	void SetupSamplers(const SamplerStateDesc samplers[], uint8_t num, uint8_t offset);
	void SetupSampler(const SamplerStateDesc& sampler, uint8_t slot);


	void SetupVertexBuffer(const VertexBufferBinding& bindings);
	void SetupIndexBuffer(const IndexBufferView* indices);
	void SetupMeshBuffers(const Mesh& bindings);
	void SetupTextures(const ResourceView* textures[], uint8_t num, uint8_t offset);
	
	void SetupRenderTargets(const RenderTargetDesc renderTargets[], int32_t num, uint8_t offset, DepthStencilView* depthStencilBuffer);
	void SetupRenderTarget(const RenderTargetDesc renderTarget, int32_t slot, DepthStencilView* depthStencilBuffer);
	void SetupDepthStencilBuffer(DepthStencilView* depthStencilBuffer);
	void SetupShader(const Shader* shader, EShaderType type);

	void SetupConstBuffers(ConstBuffer* constBuffers[], uint8_t num, uint8_t offset);
	void SetupConstBuffer(ConstBuffer* constBuffer, uint8_t slot);
	void SetupInputLayout(InputLayout* layout);


#pragma endregion

#pragma region CreateDestroyResizeResources
	/* Resources */
	
	const Resource* CreateBuffer(const BufferResourceDesc& description);
	const Resource* CreateTexture(const ResourceDesc& description);
	const Resource* CreateTexture1D(const Texture1DResourceDesc& description);
	const Resource* CreateTexture2D(const Texture2DResourceDesc& description);
	const Resource* CreateTexture3D(const Texture3DResourceDesc& description);
	const Resource* CreateTextureCube(const TextureCubeResourceDesc& description);
	
	const ShaderResourceView* CreateShaderResourceView(const ShaderResourceViewDesc& description);
	const RenderTargetView* CreateRtView(const RenderTargetViewDesc& description);
 
	const VertexBuffer* CreateVertexBuffer(const BufferResourceDesc& description);
	const IndexBuffer*  CreateIndexBuffer(const BufferResourceDesc& description);
	const ConstBuffer*  CreateConstBuffer(const BufferResourceDesc& description);
	
	const VertexBufferView* CreateVertexBufferView(const VertexBufferViewDesc& description);
	const IndexBufferView*  CreateIndexBufferView(const IndexBufferViewDesc& description);
	const ConstBufferView*  CreateConstBufferView(const ConstBufferViewDesc& description);
	
	//Resource* ResizeBuffer   (const Resource* texture, const BufferDesc& description);
	//Resource* ResizeTexture1D(const Resource* texture, const Texture1DDesc& description);
	//Resource* ResizeTexture2D(const Resource* texture, const Texture2DDesc& description);
	//Resource* ResizeTexture3D(const Resource* texture, const Texture3DDesc& description);
	

	void AddDisposeResource(const Resource* resource);
	void AddDisposeResourceView(const ResourceView* resourceView);
	//void AddDisposeConstBuffer(const IConstBuffer* constBuffers);
	//void AddDisposeVertexBuffer(const IVertexBuffer* buffer);
	//void AddDisposeIndexBuffer(const IIndexBuffer* buffer);
	
#pragma endregion

#pragma region SetResourcesData

	void SetResourceData(
		const Resource*	resource,
		uint16_t			dstSubresource,
		const UBox			rect,
		const void*			pSrcData,
		int32_t				srcRowPitch,
		int32_t				srcDepthPitch
	);
	
	void SetVertexBufferData(
		const VertexBuffer*   vertexBuffer,
		const void*			   pSrcData,
		uint32_t			   dataLength,
		int32_t				   srcRowPitch,
		int32_t				   srcDepthPitch
		);
	
	void SetIndexBufferData(
		const IndexBuffer* buffer,
		const void*			   pSrcData,
		uint32_t			   dataLength,
		int32_t				   srcRowPitch,
		int32_t				   srcDepthPitch
	);

	void SetConstBufferData(
		ConstBuffer* constBuffer,
		void* data);

#pragma endregion

	InputLayout* CreateInputLayout(const InputAssemblerDeclarationDesc& desc);
	InputLayout* CreateShader(const InputAssemblerDeclarationDesc& desc, EShaderType type);

	//void Flush();

	void BeginEvent(const char* name);
	void EndEvent();
	void SetMarker(const char* name);
    
};
}