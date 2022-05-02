#pragma once
#include "BaseStructures.h"
#include "IRenderClasses.h"


namespace GVM {

enum RenderDeviceLimitations{
	MAX_RENDERTARGET_ATTACHMENTS = 8,
	MAX_SAMPLER_ATTACHMENTS = 8,
	MAX_TEXTURE_ATTACHMENTS = 8,
	MAX_VIEWPORT_ATTACHMENTS = 8,
	MAX_CONSTBUFFER_ATTACHMENTS = 15,
};

class RenderDevice {

friend class VirtualMachine;

	RenderDevice(const IRenderDeviceInitParams* initParams, bool debugMode = true) {};
	RenderDevice(const RenderDevice&) = delete;
	RenderDevice(const RenderDevice&&) = delete;

	/* Destroys a rendering context previously returned by CreateDevice. */
	virtual ~RenderDevice() {};


	/* After your window is created, call this to check for high-DPI support.
	 *
	 * window:	The OS window handle, typically an SDL_Window*.
	 * w:		Filled with the width of the window's drawable canvas.
	 * h:		Filled with the height of the window's drawable canvas.
	 */
	
	/* Presentation */

	/* Presents the backbuffer to the window. */
	virtual void SwapBuffers() = 0;

	/* Drawing */

	virtual void ClearRenderTarget(const IRenderTarget** renderTargets, int32_t numRenderTargets, FColor color) = 0;
	virtual void Clear(float depth, int8_t stencil) = 0;
	virtual void ResetBackbuffer(const IRenderDeviceInitParams* initParams) = 0;

#pragma region DrawDispatch

	virtual void DrawIndexedPrimitives(
		int32_t baseVertex,
		int32_t minVertexIndex,
		int32_t numVertices,
		int32_t startIndex,
		int32_t primitiveCount
	) = 0;
	
	virtual void DrawInstancedPrimitives(
		int32_t baseVertex,
		int32_t minVertexIndex,
		int32_t numVertices,
		int32_t startIndex,
		int32_t primitiveCount,
		int32_t instanceCount
	) = 0;
	
	virtual void DrawPrimitives(
		int32_t vertexStart,
		int32_t primitiveCount
	) = 0;
	
	virtual void Dispatch(size_t x, size_t y, size_t z = 1) = 0;
	
#pragma endregion

#pragma region CreatePipelineStates
	virtual const IViewport* CreateViewport(const ViewportDesc* viewportDesc) = 0;
	virtual const IScissorRect* CreateScissorRect(const Rect* scissor) = 0;
	virtual const IBlendState* CreateBlendState(const BlendDesc* blendStates) = 0;
	virtual const IDepthStencilState* CreateDepthStencilState(const DepthStencilStateDesc* depthStencilState) = 0;
	virtual const IRasterizerState*  CreateRasterizerStat(const RasterizerStateDesc rasterizerState) = 0;
	virtual const ISamplerState* CreateSampler(const SamplerStateDesc* sampler) = 0;

#pragma endregion

#pragma region SetupPipelineStates
	virtual void SetViewports(const IViewport** viewport, uint8_t num) = 0;
	virtual void SetScissorRect(const IScissorRect** scissors, uint8_t num) = 0;
	virtual void SetBlendStates(const IBlendState* blendState, FColor blendFactor, uint32_t sampleMask) = 0;
	virtual void SetDepthStencilState(const IDepthStencilState* depthStencilState) = 0;
	virtual void SetRasterizerState(const IRasterizerState* rasterizerState) = 0;
	virtual void SetSamplers(const ISamplerState** sampler, uint8_t num, EShaderType shader) = 0;
	virtual void SetPipelineState(const IPipelineState pipelineState) = 0;

#pragma endregion

#pragma region SetupPipelineResources
	
	virtual void SetupTextures(const IResourceView* texture, uint8_t num, EShaderType shader) = 0;
	virtual void SetupVertexBuffer(const IVertexBuffer* bindings) = 0;
	virtual void SetRenderTargets(const IRenderTarget** renderTargets, int32_t numRenderTargets, IResourceView* depthStencilBuffer, bool isCompute = false) = 0;

#pragma endregion
	
	//virtual void ReadBackbuffer(
	//	int32_t x,
	//	int32_t y,
	//	int32_t w,
	//	int32_t h,
	//	void* data,
	//	int32_t dataLength
	//) = 0;

#pragma region CreateDestroyResizeResources
	/* Resources */
	
	virtual IResource* CreateBuffer(const BufferDesc& description) = 0;
	virtual IResource* CreateTexture1D(const Texture1DDesc& description) = 0;
	virtual IResource* CreateTexture2D(const Texture2DDesc& description) = 0;
	virtual IResource* CreateTexture3D(const Texture3DDesc& description) = 0;
	
	virtual IResource* ResizeBuffer   (const IResource* texture, const BufferDesc& description) = 0;
	virtual IResource* ResizeTexture1D(const IResource* texture, const Texture1DDesc& description) = 0;
	virtual IResource* ResizeTexture2D(const IResource* texture, const Texture2DDesc& description) = 0;
	virtual IResource* ResizeTexture3D(const IResource* texture, const Texture3DDesc& description) = 0;
	

	virtual void AddDisposeTexture(const IResource* texture) = 0;

#pragma endregion

	virtual void SetResourceData(
		const IResource* texture,
		int32_t x,
		int32_t y,
		int32_t w,
		int32_t h,
		int32_t level,
		void* data,
	) = 0;



	///* Uploads image data to a 3D texture object.
	// *
	// * texture:	The texture to be updated.
	// * x:		The x offset of the subregion being updated.
	// * y:		The y offset of the subregion being updated.
	// * z:		The z offset of the subregion being updated.
	// * w:		The width of the subregion being updated.
	// * h:		The height of the subregion being updated.
	// * d:		The depth of the subregion being updated.
	// * level:	The mipmap level being updated.
	// * data:	A pointer to the image data.
	// * dataLength:	The size of the image data in bytes.
	// */
	//virtual void SetTextureData3D(
	//	Texture* texture,
	//	int32_t x,
	//	int32_t y,
	//	int32_t z,
	//	int32_t w,
	//	int32_t h,
	//	int32_t d,
	//	int32_t level,
	//	void* data,
	//	int32_t dataLength
	//) = 0;
	//
	/* Uploads image data to a single face of a texture cube object.
	 *
	 * texture:	The texture to be updated.
	 * x:		The x offset of the subregion being updated.
	 * y:		The y offset of the subregion being updated.
	 * w:		The width of the subregion being updated.
	 * h:		The height of the subregion being updated.
	 * cubeMapFace:	The face of the cube being updated.
	 * level:	The mipmap level being updated.
	 * data:	A pointer to the image data.
	 * dataLength:	The size of the image data in bytes.
	 */
	virtual void SetTextureDataCube(
		Texture* texture,
		int32_t x,
		int32_t y,
		int32_t w,
		int32_t h,
		int32_t cubeMapFace,
		int32_t level,
		void* data,
		int32_t dataLength
	) = 0;

	///* Uploads YUV image data to three ALPHA8 texture objects.
	// *
	// * y:		The texture storing the Y data.
	// * u:		The texture storing the U (Cb) data.
	// * v:		The texture storing the V (Cr) data.
	// * yWidth:	The width of the Y plane.
	// * yHeight:	The height of the Y plane.
	// * uvWidth:	The width of the U/V planes.
	// * uvHeight:	The height of the U/V planes.
	// * data:	A pointer to the raw YUV image data.
	// * dataLength:	The size of the image data in bytes.
	// */
	//virtual void SetTextureDataYUV(
	//	Texture* y,
	//	Texture* u,
	//	Texture* v,
	//	int32_t yWidth,
	//	int32_t yHeight,
	//	int32_t uvWidth,
	//	int32_t uvHeight,
	//	void* data,
	//	int32_t dataLength
	//) = 0;

	/* Pulls image data from a 2D texture into client memory. Like any GetData,
	 * this is generally asking for a massive CPU/GPU sync point, don't call this
	 * unless there's absolutely no other way to use the image data!
	 *
	 * texture:	The texture object being read.
	 * x:		The x offset of the subregion being read.
	 * y:		The y offset of the subregion being read.
	 * w:		The width of the subregion being read.
	 * h:		The height of the subregion being read.
	 * level:	The mipmap level being read.
	 * data:	The pointer being filled with the image data.
	 * dataLength:	The size of the image data in bytes.
	 */
	virtual void GetTextureData2D(
		const Texture* texture,
		int32_t x,
		int32_t y,
		int32_t w,
		int32_t h,
		int32_t level,
		void* data,
		int32_t dataLength
	) = 0;

	///* Pulls image data from a 3D texture into client memory. Like any GetData,
	// * this is generally asking for a massive CPU/GPU sync point, don't call this
	// * unless there's absolutely no other way to use the image data!
	// *
	// * texture:	The texture object being read.
	// * x:		The x offset of the subregion being read.
	// * y:		The y offset of the subregion being read.
	// * z:		The z offset of the subregion being read.
	// * w:		The width of the subregion being read.
	// * h:		The height of the subregion being read.
	// * d:		The depth of the subregion being read.
	// * level:	The mipmap level being read.
	// * data:	The pointer being filled with the image data.
	// * dataLength:	The size of the image data in bytes.
	// */
	//virtual void GetTextureData3D(
	//	Device* device,
	//	Texture* texture,
	//	int32_t x,
	//	int32_t y,
	//	int32_t z,
	//	int32_t w,
	//	int32_t h,
	//	int32_t d,
	//	int32_t level,
	//	void* data,
	//	int32_t dataLength
	//) = 0;

	///* Pulls image data from a single face of a texture cube object into client
	// * memory. Like any GetData, this is generally asking for a massive CPU/GPU sync
	// * point, don't call this unless there's absolutely no other way to use the
	// * image data!
	// *
	// * texture:	The texture object being read.
	// * x:		The x offset of the subregion being read.
	// * y:		The y offset of the subregion being read.
	// * w:		The width of the subregion being read.
	// * h:		The height of the subregion being read.
	// * cubeMapFace:	The face of the cube being read.
	// * level:	The mipmap level being read.
	// * data:	The pointer being filled with the image data.
	// * dataLength:	The size of the image data in bytes.
	// */
	//virtual void GetTextureDataCube(
	//	Device* device,
	//	Texture* texture,
	//	int32_t x,
	//	int32_t y,
	//	int32_t w,
	//	int32_t h,
	//	CubeMapFace cubeMapFace,
	//	int32_t level,
	//	void* data,
	//	int32_t dataLength
	//) = 0;

	/* Renderbuffers */

	/* Creates a color buffer to be used by SetRenderTargets/ResolveTarget.
	 *
	 * width:		The width of the color buffer.
	 * height:		The height of the color buffer.
	 * format:		The pixel format of the color buffer.
	 * multiSampleCount:	The MSAA value for the color buffer.
	 * texture:		The texture that this buffer will be resolving to.
	 *
	 * Returns a color Renderbuffer object.
	 */
	virtual Renderbuffer* GenColorRenderbuffer(
		int32_t width,
		int32_t height,
		SurfaceFormat format,
		int32_t multiSampleCount,
		Texture* texture
	) = 0;

	/* Creates a depth/stencil buffer to be used by SetRenderTargets.
	 *
	 * width:		The width of the depth/stencil buffer.
	 * height:		The height of the depth/stencil buffer.
	 * format:		The storage format of the depth/stencil buffer.
	 * multiSampleCount:	The MSAA value for the depth/stencil buffer.
	 *
	 * Returns a depth/stencil Renderbuffer object.
	 */
	virtual Renderbuffer* GenDepthStencilRenderbuffer(
		int32_t width,
		int32_t height,
		DepthFormat format,
		int32_t multiSampleCount
	) = 0;

	/* Sends a renderbuffer to be destroyed by the renderer. Note that we call it
	 * "AddDispose" because it may not be immediately destroyed by the renderer if
	 * this is not called from the main thread (for example, if a garbage collector
	 * deletes the resource instead of the programmer).
	 *
	 * renderbuffer: The Renderbuffer to be destroyed.
	 */
	virtual void AddDisposeRenderbuffer(Renderbuffer* renderbuffer) = 0;

	/* Vertex Buffers */

	/* Creates a vertex buffer to be used by Draw*Primitives.
	 *
	 * dynamic:		Set to 1 if this buffer will be updated frequently.
	 * usage:		Set to WRITEONLY if you do not intend to call GetData.
	 * sizeInBytes:		The length of the vertex buffer.
	 * 
	 * Returns an allocated Buffer* object. Note that the contents of the
	 * buffer are undefined, so you must call SetData at least once before drawing!
	 */
	virtual Buffer* GenVertexBuffer(
		uint8_t dynamic,
		BufferUsage usage,
		int32_t sizeInBytes
	) = 0;

	/* Sends a vertex buffer to be destroyed by the renderer. Note that we call it
	 * "AddDispose" because it may not be immediately destroyed by the renderer if
	 * this is not called from the main thread (for example, if a garbage collector
	 * deletes the resource instead of the programmer).
	 *
	 * buffer: The Buffer to be destroyed.
	 */
	virtual void AddDisposeVertexBuffer(Buffer* buffer) = 0;

	/* Sets a region of the vertex buffer with client data.
	 *
	 * buffer:		The vertex buffer to be updated.
	 * offsetInBytes:	The starting offset of the buffer to write into.
	 * data:		The client data to write into the buffer.
	 * elementCount:	The number of elements from the client buffer to write.
	 * elementSizeInBytes:	The size of each element in the client buffer.
	 * vertexStride:	Try to set this to the same value as elementSizeInBytes.
	 *			XNA has this ridiculous thing where if vertexStride is
	 *			greater than elementSizeInBytes, it tries to do partial
	 *			updates of each vertex with the client data's smaller
	 *			elements. It's... just, really bad. Don't try to use it.
	 *			You probably just want '1' for both parameters, so that
	 *			elementCount can just be the buffer length in bytes.
	 * options:		Try not to call NONE if this is a dynamic buffer!
	 */
	virtual void SetVertexBufferData(
		Buffer* buffer,
		int32_t offsetInBytes,
		void* data,
		int32_t elementCount,
		int32_t elementSizeInBytes,
		int32_t vertexStride,
		SetDataOptions options
	) = 0;

	/* Pulls data from a region of the vertex buffer into a client pointer.
	 *
	 * buffer:		The vertex buffer to be read from.
	 * offsetInBytes:	The starting offset of the buffer to write into.
	 * data:		The client data to write into from the buffer.
	 * elementCount:	The number of elements from the client buffer to read.
	 * elementSizeInBytes:	The size of each element in the client buffer.
	 * vertexStride:	Try to set this to the same value as elementSizeInBytes.
	 *			XNA has this ridiculous thing where if vertexStride is
	 *			greater than elementSizeInBytes, it tries to do partial
	 *			updates of each vertex with the client data's smaller
	 *			elements. It's... just, really bad. Don't try to use it.
	 *			You probably just want '1' for both parameters, so that
	 *			elementCount can just be the buffer length in bytes.
	 */
	virtual void GetVertexBufferData(
		const Buffer* buffer,
		int32_t offsetInBytes,
		void* data,
		int32_t elementCount,
		int32_t elementSizeInBytes,
		int32_t vertexStride
	) = 0;

	/* Index Buffers */

	/* Creates an index buffer to be used by Draw*Primitives.
	 *
	 * dynamic:		Set to 1 if this buffer will be updated frequently.
	 * usage:		Set to WRITEONLY if you do not intend to call GetData.
	 * sizeInBytes:		The length of the vertex buffer.
	 *
	 * Returns an allocated Buffer* object. Note that the contents of the
	 * buffer are undefined, so you must call SetData at least once before drawing!
	 */
	virtual Buffer* GenIndexBuffer(
		uint8_t dynamic,
		BufferUsage usage,
		int32_t sizeInBytes
	) = 0;

	/* Sends an index buffer to be destroyed by the renderer. Note that we call it
	 * "AddDispose" because it may not be immediately destroyed by the renderer if
	 * this is not called from the main thread (for example, if a garbage collector
	 * deletes the resource instead of the programmer).
	 *
	 * buffer: The Buffer to be destroyed.
	 */
	virtual void AddDisposeIndexBuffer(Buffer* buffer) = 0;

	/* Sets a region of the index buffer with client data.
	 *
	 * buffer:		The index buffer to be updated.
	 * offsetInBytes:	The starting offset of the buffer to write into.
	 * data:		The client data to write into the buffer.
	 * dataLength:		The size (in bytes) of the client data.
	 * options:		Try not to call NONE if this is a dynamic buffer!
	 */
	virtual void SetIndexBufferData(
		Buffer* buffer,
		int32_t offsetInBytes,
		void* data,
		int32_t dataLength,
		SetDataOptions options
	) = 0;

	/* Pulls data from a region of the index buffer into a client pointer.
	 *
	 * buffer:		The index buffer to be read from.
	 * offsetInBytes:	The starting offset of the buffer to read from.
	 * data:		The pointer to read buffer data into.
	 * dataLength:		The size (in bytes) of the client data.
	 */
	virtual void GetIndexBufferData(
		const Buffer* buffer,
		int32_t offsetInBytes,
		void* data,
		int32_t dataLength
	) = 0;



	virtual void ApplyVertexBufferBinding(const VertexBufferBinding& bindings) = 0;
	virtual void ApplyIndexBufferBinding(const Buffer* indices, uint8_t indexElementSize) = 0;
	virtual void ApplyMeshBuffersBinding(const MeshBindings& bindings) = 0;

	struct ShaderData
	{
		void* shaderData; size_t dataSize; 
		ShaderDefines* defines; size_t definesSize; 
		void* includes;
		const char* enteryPoint;
		const char* target;
		uint16_t flags;
#ifdef _DEBUG
		const char* name;
#endif
	};

	virtual PixelShader* CompilePixelShader(
		const ShaderData& shaderData
	) = 0;

	virtual ComputeShader* CompileComputeShader(
		const ShaderData& shaderData
	) = 0;
	

	virtual GeometryShader* CompileGeometryShader(
		const ShaderData& shaderData
	) = 0;
	
	virtual VertexShader* CompileVertexShader(
		const ShaderData& shaderData,
		void* inpitLayout,
		size_t inputLayoutSize
	) = 0;

	virtual void ApplyPixelShader(PixelShader* pixelShader) = 0;
	virtual void ApplyVertexShader(VertexShader* vertexShader) = 0;
	virtual void ApplyGeometryShader(GeometryShader* vertexShader) = 0;
	virtual void ApplyComputeShader(ComputeShader* vertexShader) = 0;
	
	virtual void AddDisposePixelShader(PixelShader* pixelShader) = 0;
	virtual void AddDisposeVertexShader(VertexShader* vertexShader) = 0;
	virtual void AddDisposeGeometryShader(GeometryShader* pixelShader) = 0;
	virtual void AddDisposeComputeShader(ComputeShader* vertexShader) = 0;

	virtual ConstBuffer* CreateConstBuffer(size_t size) = 0;
	virtual void VerifyConstBuffer(ConstBuffer* constBuffer, size_t slot) = 0;
	virtual void SetConstBuffer(ConstBuffer* constBuffers, void* data) = 0;
	virtual void AddDisposeConstBuffer(ConstBuffer* constBuffers) = 0;

	virtual void ApplyPipelineState(PipelineState* piplineState) = 0;
	virtual void Flush() = 0;

	virtual void BeginEvent(const char* name) = 0;
	virtual void EndEvent() = 0;
	virtual void SetMarker(const char* name) = 0;

	virtual void ClearState() = 0;

};


}
