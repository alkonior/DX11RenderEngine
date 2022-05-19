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

	void ClearRenderTargets(const IRenderTargetView** renderTargets, int32_t numRenderTargets, FColor color);
	void ClearRenderTarget(const IRenderTargetView* renderTarget, FColor color);
	void Clear(const IDepthStencilView* septhStencil, float depth, int8_t stencil);
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
	void SetupIndexBuffer(const IIndexBufferView* indices);
	void SetupMeshBuffers(const Mesh& bindings);
	void SetupTextures(const IResourceView* textures[], uint8_t num, uint8_t offset);
	
	void SetupRenderTargets(const RenderTargetDesc renderTargets[], int32_t num, uint8_t offset, IDepthStencilView* depthStencilBuffer);
	void SetupRenderTarget(const RenderTargetDesc renderTarget, int32_t slot, IDepthStencilView* depthStencilBuffer);
	void SetupDepthStencilBuffer(IDepthStencilView* depthStencilBuffer);
	void SetupShader(const IShader* shader, EShaderType type);

	void SetupConstBuffers(IConstBuffer* constBuffers[], uint8_t num, uint8_t offset);
	void SetupConstBuffer(IConstBuffer* constBuffer, uint8_t slot);
	void SetupInputLayout(IInputLayout* layout);


#pragma endregion

#pragma region CreateDestroyResizeResources
	/* Resources */
	
	const IResource* CreateBuffer(const BufferResourceDesc& description);
	const IResource* CreateTexture(const TextureResourceDesc& description);
	const IResource* CreateTexture1D(const Texture1DResourceDesc& description);
	const IResource* CreateTexture2D(const Texture2DResourceDesc& description);
	const IResource* CreateTexture3D(const Texture3DResourceDesc& description);
	const IResource* CreateTextureCube(const TextureCubeResourceDesc& description);
	
	const IResourceView* CreateShaderResourceView(const ShaderResourceViewDesc& description);
	const IRenderTargetView* CreateRtView(const RenderTargetViewDesc& description);
 
	const IVertexBuffer* CreateVertexBuffer(const BufferResourceDesc& description);
	const IIndexBuffer*  CreateIndexBuffer(const BufferResourceDesc& description);
	const IConstBuffer*  CreateConstBuffer(const BufferResourceDesc& description);
	
	const IVertexBufferView* CreateVertexBufferView(const VertexBufferViewDesc& description);
	const IIndexBufferView*  CreateIndexBufferView(const IndexBufferViewDesc& description);
	const IConstBufferView*  CreateConstBufferView(const ConstBufferViewDesc& description);
	
	//IResource* ResizeBuffer   (const IResource* texture, const BufferDesc& description);
	//IResource* ResizeTexture1D(const IResource* texture, const Texture1DDesc& description);
	//IResource* ResizeTexture2D(const IResource* texture, const Texture2DDesc& description);
	//IResource* ResizeTexture3D(const IResource* texture, const Texture3DDesc& description);
	

	void AddDisposeResource(const IResource* resource);
	void AddDisposeResourceView(const IResourceView* resourceView);
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

	IInputLayout* CreateInputLayout(const InputAssemblerDeclarationDesc& desc);
	IInputLayout* CreateShader(const InputAssemblerDeclarationDesc& desc, EShaderType type);

	//void Flush();

	void BeginEvent(const char* name);
	void EndEvent();
	void SetMarker(const char* name);
    
};
}