#pragma once
#include "GVMBaseStructures.h"
#include "GVMBaseStructures/RenderDeviceInitParams.h"


namespace GVM {


enum RenderDeviceLimitations {
    MAX_RENDERTARGET_ATTACHMENTS = 8,
    MAX_SAMPLER_ATTACHMENTS = 8,
    MAX_TEXTURE_ATTACHMENTS = 128,
    MAX_VIEWPORT_ATTACHMENTS = 8,
    MAX_CONSTBUFFER_ATTACHMENTS = 15,
};

class IRenderDevice {
public:
    IRenderDevice(const IRenderDevice&) = delete;
    IRenderDevice(const IRenderDevice&&) = delete;
    virtual ~IRenderDevice() = 0 {}
    virtual void GetBackbufferSize(uint32_t& w, uint32_t& h) = 0;

    struct IPlaceable {
    public:
        //virtual void Place(void* ptr) const = 0;
        //virtual ~IPlaceable() = 0 {};
    };


    class IResource : public IPlaceable {};
    class IResourceView : public IPlaceable {};
    class IInputLayout : public IPlaceable {};
    class IShader : public IPlaceable {};

protected:
    class IVertexBufferView : public IResourceView {};
    class IIndexBufferView : public IResourceView {};
    class IConstBufferView : public IResourceView {};
    class IRenderTargetView : public IResourceView {};
    class IDepthStencilView : public IResourceView {};
    class IShaderResourceView : public IResourceView {};
    class IUATargetView : public IResourceView {};


    friend class VirtualMachine;
    friend class ResourcesManager;
    friend class GpuResource;
    friend class GpuResourceView;

    IRenderDevice(const RenderDeviceInitParams& initParams, bool debugMode = true) {}

    virtual void Draw(const DrawCall& call) = 0;
    virtual void Present() = 0;


    virtual IResource* CreateResource(const GpuResource& resource) = 0;
    virtual void DestroyResource(IPlaceable* resource) = 0;
    virtual IResourceView* CreateResourceView(const GpuResourceView& desc, const GpuResource& ResourceDesc) = 0;



    virtual IShader* CreateShader(const ShaderDesc& desc) = 0;
    virtual IInputLayout* CreateInputLayout(const InputAssemblerDeclarationDesc& desc, const ShaderDesc& Shader) = 0;
    virtual void ClearState() = 0;


    virtual void BeginEvent(const char* name) = 0;
    virtual void EndEvent() = 0;

    
    virtual void* GetNativeTexture(const IResourceView* view) = 0;

    virtual void SetResourceData(
        const GpuResource& resource,
        uint16_t dstSubresource,
        const UBox& rect,
        const void* pSrcData,
        int32_t srcRowPitch = 0,
        int32_t srcDepthPitch = 0
    ) = 0;


    virtual void SyncBlockExecutionStart() = 0;
    virtual void SyncResourcesRead(IResource** data, size_t size) = 0;
    virtual void SyncResourcesWrite(IResource** data, size_t size) = 0;
    virtual void SyncBlockExecutionEnd() = 0;

#pragma region SetupPipeline

    virtual void SetupViewports(const Compressed::ViewportDesc viewport[], uint8_t num) = 0;
    virtual void SetupBlendState(const Compressed::CoreBlendDesc& blendState) = 0;
    virtual void SetupDepthStencilState(const Compressed::DepthStencilStateDesc& depthStencilState) = 0;
    virtual void SetupRasterizerState(const Compressed::RasterizerStateDesc& rasterizerState) = 0;
    virtual void SetupSamplers(const Compressed::SamplerStateDesc samplers[], uint8_t num) = 0;
    virtual void SetupPrimitiveTopology(const EPrimitiveTopology topology) = 0;

    virtual void SetupVertexBuffer(const IVertexBufferView* vertexBuffers[], uint8_t num) = 0;
    virtual void SetupIndexBuffer(const IIndexBufferView* indices) = 0;
    virtual void SetupTextures(IResourceView* textures[], uint8_t num) = 0;
    virtual void SetupRenderTargets(const IRenderTargetView* renderTargets[], int32_t num, IDepthStencilView* depthStencilBuffer) = 0;
    virtual void SetupUATargets(const ::GVM::IRenderDevice::IUATargetView** ua_targets, uint8_t uint8) = 0;

    virtual void SetupShader(IShader* shader, EShaderType type) = 0;
    virtual void SetupConstBuffers(IConstBufferView* constBuffers[], uint8_t num) = 0;
    virtual void SetupInputLayout(IInputLayout* layout) = 0;
    virtual void ClearRenderTarget(const IRenderTargetView* rtView, FColor color) = 0;
    virtual void ClearDepthStencil(const IDepthStencilView* dsView, float depth, int8_t stencil) = 0;


#pragma endregion


    virtual void ResizeBackbuffer(int32_t width, int32_t height) = 0;

    /*
    virtual ~IRenderDevice() = default;


    /+* Presentation *+/

    virtual void SwapBuffers() = 0;

    /+* Drawing *+/

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
//#pragma region CreateDestroyPipelineStates
//    virtual const IViewport* CreateViewport(const ViewportDesc* viewportDesc) = 0;
//    virtual const IScissorRect* CreateScissorRect(const Rect* scissor) = 0;
//    virtual const IBlendState* CreateBlendState(const BlendDesc* blendStates) = 0;
//    virtual const IDepthStencilState* CreateDepthStencilState(const DepthStencilStateDesc* depthStencilState) = 0;
//    virtual const IRasterizerState* CreateRasterizerStat(const RasterizerStateDesc* rasterizerState) = 0;
//    virtual const ISamplerState* CreateSampler(const SamplerStateDesc* sampler) = 0;
//    virtual IInputLayout* CreateInputLayout(const InputAssemblerDeclarationDesc& inputAssemblerDesc) = 0;
//    virtual IShader* CreateShader(const ShaderDesc& shaderDesc, EShaderType type) = 0;
//
//    virtual void AddDisposeViewport(const IViewport* viewportDesc) = 0;
//    virtual void AddDisposeScissorRect(const IScissorRect* scissor) = 0;
//    virtual void AddDisposeBlendState(const IBlendState* blendStates) = 0;
//    virtual void AddDisposeDepthStencilState(const IDepthStencilState* depthStencilState) = 0;
//    virtual void AddDisposeRasterizerStat(const IRasterizerState* rasterizerState) = 0;
//    virtual void AddDisposeSampler(const ISamplerState* sampler) = 0;
//    virtual void AddDisposeInputLayout(const IInputLayout*) = 0;
//    virtual void AddDisposeShader(IShader* shader) = 0;
//#pragma endregion

#pragma region SetupPipeline
    virtual void SetViewports(const IViewport** viewport, uint8_t num) = 0;
    virtual void SetScissorRect(const IScissorRect** scissors, uint8_t num) = 0;
    virtual void SetBlendStates(const IBlendState* blendState, FColor blendFactor, uint32_t sampleMask) = 0;
    virtual void SetDepthStencilState(const IDepthStencilState* depthStencilState) = 0;
    virtual void SetRasterizerState(const IRasterizerState* rasterizerState) = 0;
    virtual void SetSamplers(const ISamplerState** sampler, uint8_t num, EShaderType shader) = 0;
    virtual void SetInputLayout(const IInputLayout*) = 0;


    virtual void SetupVertexBuffer(const VertexBufferBinding& bindings) = 0;
    virtual void SetupIndexBuffer(const IIndexBufferView* indices) = 0;
    virtual void SetupMeshBuffers(const Mesh& bindings) = 0;


    virtual void SetupTextures(const IResourceView* texture, uint8_t num, EShaderType shader) = 0;
    virtual void SetupVertexBuffer(const IVertexBuffer* bindings) = 0;
    virtual void SetupRenderTargets(const IRenderTarget** renderTargets, int32_t numRenderTargets, IResourceView* depthStencilBuffer, bool isCompute = false) = 0;
    virtual void SetupShader(const IShader* shader, EShaderType type) = 0;

    virtual void SetupConstBuffers(IConstBuffer** constBuffers, size_t num) = 0;

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
    /+* Resources *+/

    virtual IResource* CreateBuffer(const BufferDesc& description) = 0;
    virtual IResource* CreateTexture1D(const Texture1DDesc& description) = 0;
    virtual IResource* CreateTexture2D(const Texture2DDesc& description) = 0;
    virtual IResource* CreateTexture3D(const Texture3DDesc& description) = 0;

    virtual IResourceView* CreateResourceView(const IResource* resource, const ResourceViewDesc& description) = 0;

    virtual IVertexBuffer* CreateVertexBuffer(const VertexBufferDesc& description) = 0;
    virtual IIndexBuffer* CreateIndexBuffer(const IndexBufferDesc& description) = 0;

    virtual IResource* ResizeBuffer(const IResource* texture, const BufferDesc& description) = 0;
    virtual IResource* ResizeTexture1D(const IResource* texture, const Texture1DDesc& description) = 0;
    virtual IResource* ResizeTexture2D(const IResource* texture, const Texture2DDesc& description) = 0;
    virtual IResource* ResizeTexture3D(const IResource* texture, const Texture3DDesc& description) = 0;


    virtual void AddDisposeResource(const IResource* texture) = 0;
    //virtual void AddDisposeConstBuffer(const IConstBuffer* constBuffers) = 0;
    //virtual void AddDisposeVertexBuffer(const IVertexBuffer* buffer) = 0;
    //virtual void AddDisposeIndexBuffer(const IIndexBuffer* buffer) = 0;

#pragma endregion
#pragma region SetResources

    virtual void SetResourceData(
        const IResource* resource,
        uint16_t dstSubresource,
        const UBox rect,
        const void* pSrcData,
        int32_t srcRowPitch,
        int32_t srcDepthPitch
    ) = 0;

    virtual void SetVertexBufferData(
        const IVertexBuffer* vertexBuffer,
        const void* pSrcData,
        uint32_t dataLength,
        int32_t srcRowPitch,
        int32_t srcDepthPitch
    ) = 0;

    virtual void SetIndexBufferData(
        const IIndexBuffer* buffer,
        const void* pSrcData,
        uint32_t dataLength,
        int32_t srcRowPitch,
        int32_t srcDepthPitch
    ) = 0;

    virtual void SetConstBufferData(
        IConstBuffer* constBuffer,
        void* data) = 0;
#pragma endregion


    virtual void Flush() = 0;

    virtual void BeginEvent(const char* name) = 0;
    virtual void EndEvent() = 0;
    virtual void SetMarker(const char* name) = 0;

    virtual void ClearState() = 0;
*/
};


}
