#pragma once

#define GVM_MSW

#include "winHandlerdx12.h"
#define DX12
#include "IRenderDevice.h"
#include "GraphicsExceptions/DxgiInfoManager.h"

#include "SID.h"


namespace DirectX {
class DescriptorHeap;
}
namespace GVM {
class ResourceViewDX12;

struct Dx12PlatformHandle : public PlatformHandle {

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
    uint8_t d3d11viewportsNum = 0;

    static const int SwapChainBufferCount = 2;
    int mCurrBackBuffer = 0;
    Microsoft::WRL::ComPtr<ID3D12Resource> mSwapChainBuffer[SwapChainBufferCount];
    UINT BackBufferWidth = 0;
    UINT BackBufferHeight = 0;

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mBBHeap;
    
    DirectX::DescriptorHeap* mDsvHeapInterface;
    DirectX::DescriptorHeap* mRtvHeapInterface;
    DirectX::DescriptorHeap* mGpuSamplerHeapInterface;
    DirectX::DescriptorHeap* mGpuShvCbUaHeapInterface;
    

    
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
    std::vector<D3D12_INPUT_LAYOUT_DESC> inputLayouts;
    std::vector<std::vector<D3D12_INPUT_ELEMENT_DESC>> inputLayoutsData;
    std::vector<D3D12_SHADER_BYTECODE> shaders;

    Microsoft::WRL::ComPtr<ID3D12Resource> cbResource;
    D3D12_CPU_DESCRIPTOR_HANDLE defaultCB;
    D3D12_CPU_DESCRIPTOR_HANDLE defaultSampler;
    Microsoft::WRL::ComPtr<ID3D12Resource> shResource;
    D3D12_CPU_DESCRIPTOR_HANDLE defaultTexture;
    Microsoft::WRL::ComPtr<ID3D12Resource> rtResource;
    D3D12_CPU_DESCRIPTOR_HANDLE defaultRT[8];
    Microsoft::WRL::ComPtr<ID3D12Resource> uaResource;
    D3D12_CPU_DESCRIPTOR_HANDLE defaultUA[8];

    
    std::vector<D3D12_VERTEX_BUFFER_VIEW> vertexBuffersPtr;
    size_t vertexBuffersNum = 0;

    // Derived class should set these in derived constructor to customize starting values.
    D3D_DRIVER_TYPE md3dDriverType = D3D_DRIVER_TYPE_HARDWARE;
    DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    UINT      m4xMsaaQuality = 0;      // quality level of 4X MSAA

    std::vector<std::pair<uint32_t, wrl::ComPtr<ID3D12RootSignature>>> RootSignatures = {};
#pragma region Fields

#ifdef _DEBUG
    DxgiInfoManager infoManager;
#endif
    
    HRESULT hr;
    
    
    bool CS = false;
    bool PS = false;
    bool VS = false;
    bool GS = false;
    bool HS = false;
    bool DS = false;

#pragma endregion 
private:
    
#ifdef _DEBUG
    void LogAdapters();
    void LogAdapterOutput(IDXGIAdapter* Adapter);
    void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);
#endif
    
    void CreateCommandObjects(const RenderDeviceInitParams& initParams);
    void CreateSwapChain(const RenderDeviceInitParams& initParams);
    ID3D12Resource* CurrentBackBuffer() const;
    D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView();
    void CreateRtvAndDsvDescriptorHeaps();
    void CreateDefaultHandles();

    
protected:

    IResource* CreateResource(const GpuResource& ResourceDesc) override;
public:
    void DestroyResource(const RESOURCEHANDLE resource) override;
    void DestroyResourceView(const RESOURCEVIEWHANDLE resource) override;
    
    SHADERHANDLE CreateShader(const ShaderDesc& desc) override;
    INPUTLAYOUTHANDLE CreateInputLayout(const InputAssemblerDeclarationDesc& desc, const ShaderDesc& Shader) override;
    RESOURCEVIEWHANDLE CreateResourceView(const GpuResourceView& desc, const GpuResource& ResourceDesc) override;

    void* GetNativeTexture(RESOURCEVIEWHANDLE view) override;
private:
    std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> uploadTextureBuffers;
    std::map<string_id, Microsoft::WRL::ComPtr<ID3D12PipelineState>> pipelineStates;

    std::unordered_map<uint64_t, D3D12_CPU_DESCRIPTOR_HANDLE>hashSS;
    D3D12_CPU_DESCRIPTOR_HANDLE FetchSamplerState(const Compressed::SamplerStateDesc& state);
    
    struct SignatureParams {
        union {
            struct {
                uint8_t cbCount;
                uint8_t srvCount;
                uint8_t uaCount;
                uint8_t samplersCount;
            };
            uint32_t data;
        };
    };
    ID3D12RootSignature* FetchRS(SignatureParams params);
public:
    
    void UploadSubresourceData(const GpuResource& resource, uint16_t dstSubresource, const size_t dataSize,  const void* pSrcData, int32_t srcRowPitch, int32_t srcDepthPitch) override;
private:
    std::vector<wrl::ComPtr<ID3D12Resource>> bufferUploadBuffers;
    wrl::ComPtr<ID3D12Resource> currentBufferUploadBuffer;
    size_t currentBufferUploadBufferSize = 0;
    size_t currentBufferUploadBufferShift = 0;
    BYTE* currentBufferUploadBufferPtr = nullptr;

    void MakeUploadBufferBigger(size_t minSize);

    
public:
    
    void SetSubresourceData(const GpuResource& resource, uint16_t dstSubresource, const UBox& rect, const void* pSrcData, int32_t srcRowPitch, int32_t srcDepthPitch) override;

#pragma region ResourceSynchronization

    void SyncBlockExecutionStart() override
    { }
    void SyncResourcesState(GpuResource * data[], size_t size) override;
    void SyncBlockExecutionEnd() override{};

#pragma endregion

#pragma region PipelineSetup
    
    void SetupPipeline(const PipelineDescription& Pipeline) override;
    void SetupVertexBuffers(const VERTEXBUFFERVIEWHANDLE vertexBuffers[], uint8_t num) override;
    void SetupIndexBuffer(const INDEXBUFFERVIEWHANDLE indices) override;
    void SetupTextures(RESOURCEVIEWHANDLE textures[], uint8_t num) override;
    void SetupRenderTargets(const RENDERTARGETVIEWHANDLE renderTargets[], int32_t num, DEPTHSTENCILVIEWHANDLE depthStencilBuffer) override;
    void SetupUATargets(UATARGETVIEWHANDLE ua_targets[], uint8_t uint8) override;
    void SetupConstBuffers(CONSTBUFFERVIEWHANDLE constBuffers[], uint8_t num) override;
    void ClearState() override;
    void SetupViewports(const Compressed::ViewportDesc viewports[], uint8_t num);
    void SetupSamplers(const Compressed::SamplerStateDesc samplers[], uint8_t num);
    Compressed::SamplerStateDesc  cachedSamplers[20];
    uint8_t cachedSamplersNUm = 0;
    D3D12_GPU_DESCRIPTOR_HANDLE cachedSamplerhandle;
#pragma endregion

    void ClearRenderTarget(RENDERTARGETVIEWHANDLE rtView, FColor color) override;
    void ClearDepthStencil(DEPTHSTENCILVIEWHANDLE dsView, float depth, int8_t stencil) override;
    void FlushCommandQueue();
    void Present() override;
    void Draw(const DrawCall& call) override;
    void ResizeBackbuffer(int32_t width, int32_t height) override{};


    
    void GetBackbufferSize(uint32_t& w, uint32_t& h) override;
    
    void BeginEvent(const char* name) override;
    void EndEvent() override;
    
};

}
