#pragma once

#define GVM_MSW

#include "winHandler.h"
#include "IRenderDevice.h"
#include "GraphicsExceptions/DxgiInfoManager.h"



namespace GVM {
class ResourceViewDX12;

struct Dx11PlatformHandle : public PlatformHandle {

#if defined(GVM_LINUX)
//xcb_connection_t*                   connection;
xcb_window_t                        window;
#elif defined(GVM_MSW)
//HINSTANCE                           hinstance;
HWND                                hwnd;
#endif

};

class RenderDeviceDX12 final : public IRenderDevice {
public:
    RenderDeviceDX12(const RenderDeviceInitParams& initParams, bool debugMode);
    RenderDeviceDX12(const IRenderDevice& render_device) = delete;
    RenderDeviceDX12(const IRenderDevice&& render_device) = delete;
    ~RenderDeviceDX12() override;

    Microsoft::WRL::ComPtr<IDXGIFactory4> mdxgiFactory;
    Microsoft::WRL::ComPtr<IDXGISwapChain> mSwapChain;
    Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice;

    Microsoft::WRL::ComPtr<ID3D12Fence> mFence;
    UINT64 mCurrentFence = 0;
	
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCommandQueue;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mDirectCmdListAlloc;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList;

    static const int SwapChainBufferCount = 2;
    int mCurrBackBuffer = 0;
    Microsoft::WRL::ComPtr<ID3D12Resource> mSwapChainBuffer[SwapChainBufferCount];
    Microsoft::WRL::ComPtr<ID3D12Resource> mDepthStencilBuffer;

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mRtvHeap;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDsvHeap;

    D3D12_VIEWPORT mScreenViewport; 
    D3D12_RECT mScissorRect;

    UINT mRtvDescriptorSize = 0;
    UINT mDsvDescriptorSize = 0;
    UINT mCbvSrvUavDescriptorSize = 0;

    // Derived class should set these in derived constructor to customize starting values.
    D3D_DRIVER_TYPE md3dDriverType = D3D_DRIVER_TYPE_HARDWARE;
    DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    UINT      m4xMsaaQuality = 0;      // quality level of 4X MSAA

#pragma region Fields

#ifdef _DEBUG
    DxgiInfoManager infoManager;
#endif
    
    HRESULT hr;
    
    
    wrl::ComPtr<ID3D12Device> device;
    bool CS = false;
    bool PS = false;
    bool VS = false;
    bool GS = false;
    bool HS = false;
    bool DS = false;

private:
    
#ifdef _DEBUG
    void LogAdapters();
    void LogAdapterOutput(IDXGIAdapter* Adapter);
    void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);
#endif
    
    void CreateCommandObjects(const RenderDeviceInitParams& initParams);
    void CreateSwapChain(const RenderDeviceInitParams& initParams);
    void CreateRtvAndDsvDescriptorHeaps();
#pragma endregion 

    
protected:

    IResource* CreateResource(const GpuResource& ResourceDesc) override;
    void DestroyResource(IPlaceable* resource) override;
    IResourceView* CreateResourceView(const GpuResourceView& desc, const GpuResource& ResourceDesc) override;

    IShader* CreateShader(const ShaderDesc& desc) override;
    IInputLayout* CreateInputLayout(const InputAssemblerDeclarationDesc& desc, const ShaderDesc& Shader) override;
    
    void SetResourceData(const GpuResource& resource, uint16_t dstSubresource, const UBox& rect, const void* pSrcData, int32_t srcRowPitch, int32_t srcDepthPitch) override;

     virtual void Draw(const DrawCall& call) override;
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
    virtual void SetupUATargets(const IUATargetView* uaTargets[], uint8_t num) override;
    
    virtual void SetupShader(IShader* shader, EShaderType type);
    virtual void SetupConstBuffers(IConstBufferView* constBuffers[], uint8_t num);
    void SetupInputLayout(IInputLayout* layout) override;
    void ClearState() override;
    
    void ClearRenderTarget(const IRenderTargetView* rtView, FColor color) override;
    void ClearDepthStencil(const IDepthStencilView* dsView, float depth, int8_t stencil) override;


#pragma endregion

    void ResizeBackbuffer(int32_t width, int32_t height) override;

    void BeginEvent(const char* name) override;
    void EndEvent() override;

    void* GetNativeTexture(const IResourceView* view) override;
public:
    
    void GetBackbufferSize(uint32_t& w, uint32_t& h) override;
};

}
