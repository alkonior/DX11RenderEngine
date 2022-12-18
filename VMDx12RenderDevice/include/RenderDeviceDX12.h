#pragma once

#define GVM_MSW

#include "winHandler.h"
#define DX12
#include "DescriptorHeap.h"
#include "IRenderDevice.h"
#include "GraphicsExceptions/DxgiInfoManager.h"
#undef DX12


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

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mBBHeap;
    
    DirectX::DescriptorHeap* mDsvHeapInterface;
    DirectX::DescriptorHeap* mRTVHeapInterface;
    DirectX::DescriptorHeap* mSamplerHeapInterface;
    DirectX::DescriptorHeap* mShvHeapInterface;
    DirectX::DescriptorHeap* mCbHeapInterface;
    DirectX::DescriptorHeap* mUaHeapInterface;
    
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;

    
    DirectX::DescriptorHeap* srvStorageHeap;
    DirectX::DescriptorHeap* rtvStorageHeap;
    std::map<uint64_t, DXGI_FORMAT> descriptorFormats;
    DirectX::DescriptorHeap* samplersStorageHeap;

    D3D12_VIEWPORT mScreenViewport; 
    D3D12_RECT mScissorRect;

    UINT mRtvDescriptorSize = 0;
    UINT mDsvDescriptorSize = 0;
    UINT mCbvSrvUavDescriptorSize = 0;
    UINT mSamplerDescriptorSize = 0;

    std::vector<D3D12_INDEX_BUFFER_VIEW> indexBuffers;
    std::vector<D3D12_VERTEX_BUFFER_VIEW> vertexBuffers;
    

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
    D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView() const;
#pragma endregion 

    
protected:

    IResource* CreateResource(const GpuResource& ResourceDesc) override;
public:
    void DestroyResource(const RESOURCEHANDLE resource) override;
    void DestroyResourceView(const RESOURCEVIEWHANDLE resource) override;
    
    SHADERHANDLE CreateShader(const ShaderDesc& desc) override;
    INPUTLAYOUTHANDLE CreateInputLayout(const InputAssemblerDeclarationDesc& desc, const ShaderDesc& Shader) override;
    RESOURCEVIEWHANDLE CreateResourceView(const GpuResourceView& desc, const GpuResource& ResourceDesc) override;

    void* GetNativeTexture(RESOURCEVIEWHANDLE view) override;
    void SetResourceData(const GpuResource& resource, uint16_t dstSubresource, const UBox& rect, const void* pSrcData, int32_t srcRowPitch, int32_t srcDepthPitch) override;

#pragma region ResourceSynchronization

    void SyncBlockExecutionStart() override;
    void SyncResourcesRead(RESOURCEHANDLE data[], size_t size) override;
    void SyncResourcesWrite(RESOURCEHANDLE data[], size_t size) override;
    void SyncBlockExecutionEnd() override;
#pragma endregion

#pragma region PipelineSetup
    
    void SetupPipeline(const PipelineDescription& Pipeline) override;
    void SetupVertexBuffers(const VERTEXBUFFERVIEWHANDLE vertexBuffers[], uint8_t num) override;
    void SetupIndexBuffers(const INDEXBUFFERVIEWHANDLE indices) override;
    void SetupTextures(RESOURCEVIEWHANDLE textures[], uint8_t num) override;
    void SetupRenderTargets(const RENDERTARGETVIEWHANDLE renderTargets[], int32_t num, DEPTHSTENCILVIEWHANDLE depthStencilBuffer) override;
    void SetupUATargets(UATARGETVIEWHANDLE ua_targets[], uint8_t uint8) override;
    void SetupConstBuffers(CONSTBUFFERVIEWHANDLE constBuffers[], uint8_t num) override;
    void ClearState() override;
    
#pragma endregion

    void ClearRenderTarget(RENDERTARGETVIEWHANDLE rtView, FColor color) override;
    void ClearDepthStencil(DEPTHSTENCILVIEWHANDLE dsView, float depth, int8_t stencil) override;
    void Present() override;
    void Draw(const DrawCall& call) override;
    void ResizeBackbuffer(int32_t width, int32_t height) override;


    
    void GetBackbufferSize(uint32_t& w, uint32_t& h) override;
    
    void BeginEvent(const char* name) override;
    void EndEvent() override;
    
};

}
