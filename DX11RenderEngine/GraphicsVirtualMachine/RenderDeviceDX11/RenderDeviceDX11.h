#pragma once
#include "pch.h"

#define GVM_MSW

#include "../VirtualMachine/IRenderDevice.h"
#include "BaseDX11Classes.h"
#include "../GraphicsExceptions/DxgiInfoManager.h"


namespace GVM {

class RenderDeviceDX11 final : public IRenderDevice {
public:

    RenderDeviceDX11(const RenderDeviceInitParams& initParams, bool debugMode);
    RenderDeviceDX11(const IRenderDevice& render_device) = delete;
    RenderDeviceDX11(const IRenderDevice&& render_device) = delete;
    ~RenderDeviceDX11() override;



#pragma region Fields

#ifdef _DEBUG
    DxgiInfoManager infoManager;
#endif
    HRESULT hr;
    
    
    wrl::ComPtr<ID3D11Device> device;
    wrl::ComPtr<ID3D11DeviceContext> context;
    wrl::ComPtr<IDXGISwapChain> swapchain;

private:
    wrl::ComPtr<ID3DUserDefinedAnnotation> perf;
    wrl::ComPtr<IDXGIAdapter> adapter;
    wrl::ComPtr<IDXGIFactory> factory;
    D3D_FEATURE_LEVEL featureLevel;

    /* Capabilities */
    uint8_t debugMode;
    //uint32_t supportsDxt1;
    //uint32_t supportsS3tc;
    //int32_t maxMultiSampleCount;
    //D3D_FEATURE_LEVEL featureLevel;

    /* Presentation */
    uint8_t syncInterval;

    //D3D11Backbuffer backbuffer;
    //uint8_t backbufferSizeChanged;
    //Rect prevSrcRect;
    //Rect prevDstRect;

    /* Blend State */
    wrl::ComPtr<ID3D11BlendState> blendState;
    FColor blendFactor;
    uint32_t multiSampleMask;

    /* Depth Stencil State */
    wrl::ComPtr<ID3D11DepthStencilState> depthStencilState;
    int32_t stencilRef;

    /* Rasterizer State */
    D3D11_VIEWPORT viewport[10];
    int maxViewportSlot;
    Rect scissorRect;
    wrl::ComPtr<ID3D11RasterizerState> rasterizerState;

    /* Textures */
    std::vector<ID3D11ShaderResourceView*> Textures;

    /* Input Assembly */
    InputLayoutDX11 inputLayout;
    EPrimitiveTopology topology;
    std::vector<ID3D11Buffer*> vertexBuffers;
    std::vector<uint32_t> vertexBufferOffsets;
    std::vector<uint32_t> vertexBufferStrides;
    ResourceViewDX11* indexBuffer;
    ResourceViewDX11* vertexBuffer;
    size_t indexElementSize;

    ///* Resource Caches */
    //PackedStateArray blendStateCache;
    //PackedStateArray depthStencilStateCache;
    //PackedStateArray rasterizerStateCache;
    //PackedStateArray samplerStateCache;
    //PackedVertexBufferBindingsArray inputLayoutCache;

    /* Render Targets */
    int32_t numRenderTargets;
    size_t backBufferWidth;
    size_t backBufferHeight;
    ID3D11RenderTargetView* swapchainRTView;
    ID3D11UnorderedAccessView* swapchainUAView;
    std::vector<ID3D11RenderTargetView*> renderTargetViews;
    std::vector<ID3D11UnorderedAccessView*> uaViews;
	
    ID3D11DepthStencilView* depthStencilBuffer;


    std::mutex ctxLock;

    std::unordered_map<uint64_t, ID3D11BlendState*>		    hashBS;
    std::unordered_map<uint64_t, ID3D11DepthStencilState*>  hashDSS;
    std::unordered_map<uint64_t, ID3D11RasterizerState*>	hashRS;
    std::unordered_map<uint64_t, ID3D11SamplerState*>	    hashSS;

    ID3D11BlendState* FetchBlendState(const Compressed::CoreBlendDesc& blendState);
    ID3D11DepthStencilState* FetchDepthStencilState(const Compressed::DepthStencilStateDesc& depthStencilState);
    ID3D11RasterizerState*FetchRasterizerState(const Compressed::RasterizerStateDesc& rasterizerState);
    ID3D11SamplerState* FetchSamplerState(const Compressed::SamplerStateDesc& state);

#pragma endregion 

    
protected:
    IStructuresSize GetClassesSize() override;

    IResource* CreateResource(const GpuResource& ResourceDesc) override;
    void DestroyResource(IResource* resource) override;
    IResourceView* CreateResourceView(const GpuResourceView& desc, const GpuResource& ResourceDesc) override;

    IShader* CreateShader(const ShaderDesc& desc) override;
    IInputLayout* CreateInputLayout(const InputAssemblerDeclarationDesc& desc, const ShaderDesc& Shader) override;
    
    void SetResourceData(const GpuResource& resource, uint16_t dstSubresource, const UBox& rect, const void* pSrcData, int32_t srcRowPitch, int32_t srcDepthPitch) override;

     virtual void Draw(DrawCall call) override;
    void Present() override;

#pragma region SetupPipeline
    
    virtual void SetupViewports(const Compressed::ViewportDesc viewports[], uint8_t num);
    virtual void SetupBlendState(const Compressed::CoreBlendDesc& blendState);
    virtual void SetupDepthStencilState(const Compressed::DepthStencilStateDesc& depthStencilState);
    virtual void SetupRasterizerState(const Compressed::RasterizerStateDesc& rasterizerState);
    virtual void SetupSamplers(const Compressed::SamplerStateDesc samplers[], uint8_t num);
    virtual void SetupPrimitiveTopology(const EPrimitiveTopology topology);

    virtual void SetupVertexBuffer(const IVertexBufferView* vertexBuffers[], uint8_t num);
    virtual void SetupIndexBuffer(const IIndexBufferView* indices);
    virtual void SetupTextures(IResourceView* textures[], uint8_t num);
    virtual void SetupRenderTargets(const IRenderTargetView* renderTargets[], int32_t num, IDepthStencilView* depthStencilBuffer);
    
    virtual void SetupShader(IShader* shader, EShaderType type);
    virtual void SetupConstBuffers(IConstBufferView* constBuffers[], uint8_t num);
    void SetupInputLayout(IInputLayout* layout) override;
  


#pragma endregion

    
};

}
