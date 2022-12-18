#include "RenderDeviceDX12.h"
#include "../DX12Helpers/d3dUtil.h"
#include "GPUResourcesDescription/GPUResource.h"
#include "GVMToDx12.h"

template <class T>
using ComPtr = wrl::ComPtr<T>;
using namespace GVM;

enum RootParameterIndex
{
    UABase,
    SRVBase,
    SamplerBase,
    CBBase,
    RootParameterCount
};

enum DX12Limitations
{
    maxDSV = 1,
    maxRTV = 8,
    maUA = 8,
    maxTextures = 40,
    maxConstBuffers = 16,
    maxSamplers = 20,
};


RenderDeviceDX12::RenderDeviceDX12(const RenderDeviceInitParams& initParams, bool debugMode): IRenderDevice(
    initParams, debugMode)
{
#if defined(DEBUG) || defined(_DEBUG)
    // Enable the D3D12 debug layer.
    {
        ComPtr<ID3D12Debug> debugController;
        ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
        debugController->EnableDebugLayer();
    }
#endif

    ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&mdxgiFactory)));

    // Try to create hardware device.
    HRESULT hardwareResult = D3D12CreateDevice(
        nullptr, // default adapter
        D3D_FEATURE_LEVEL_12_0,
        IID_PPV_ARGS(&md3dDevice));

    // Fallback to WARP device.
    if (FAILED(hardwareResult))
    {
        ComPtr<IDXGIAdapter> pWarpAdapter;
        ThrowIfFailed(mdxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter)));

        ThrowIfFailed(D3D12CreateDevice(
            pWarpAdapter.Get(),
            D3D_FEATURE_LEVEL_11_0,
            IID_PPV_ARGS(&md3dDevice)));
    }

    ThrowIfFailed(md3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE,
        IID_PPV_ARGS(&mFence)));

    mRtvDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    mDsvDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    mCbvSrvUavDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    mSamplerDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

    // Check 4X MSAA quality support for our back buffer format.
    // All Direct3D 11 capable devices support 4X MSAA for all render 
    // target formats, so we only need to check quality support.

    D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
    msQualityLevels.Format = mBackBufferFormat;
    msQualityLevels.SampleCount = 4;
    msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
    msQualityLevels.NumQualityLevels = 0;
    ThrowIfFailed(md3dDevice->CheckFeatureSupport(
        D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
        &msQualityLevels,
        sizeof(msQualityLevels)));

    m4xMsaaQuality = msQualityLevels.NumQualityLevels;
    assert(m4xMsaaQuality > 0 && "Unexpected MSAA quality level.");

#ifdef _DEBUG
    LogAdapters();
#endif

    CreateCommandObjects(initParams);
    CreateSwapChain(initParams);
    CreateRtvAndDsvDescriptorHeaps();

    return;
}


RenderDeviceDX12::~RenderDeviceDX12()
{
}

#ifdef _DEBUG
void RenderDeviceDX12::LogAdapterOutput(IDXGIAdapter* adapter)
{
    UINT i = 0;
    IDXGIOutput* output = nullptr;
    while (adapter->EnumOutputs(i, &output) != DXGI_ERROR_NOT_FOUND)
    {
        DXGI_OUTPUT_DESC desc;
        output->GetDesc(&desc);

        std::wstring text = L"***Output: ";
        text += desc.DeviceName;
        text += L"\n";
        OutputDebugString(text.c_str());

        LogOutputDisplayModes(output, mBackBufferFormat);

        ReleaseCom(output);

        ++i;
    }
}

void RenderDeviceDX12::LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format)
{
    UINT count = 0;
    UINT flags = 0;

    // Call with nullptr to get list count.
    output->GetDisplayModeList(format, flags, &count, nullptr);

    std::vector<DXGI_MODE_DESC> modeList(count);
    output->GetDisplayModeList(format, flags, &count, &modeList[0]);

    for (auto& x : modeList)
    {
        UINT n = x.RefreshRate.Numerator;
        UINT d = x.RefreshRate.Denominator;
        std::wstring text =
            L"Width = " + std::to_wstring(x.Width) + L" " +
            L"Height = " + std::to_wstring(x.Height) + L" " +
            L"Refresh = " + std::to_wstring(n) + L"/" + std::to_wstring(d) +
            L"\n";

        ::OutputDebugString(text.c_str());
    }
}

void RenderDeviceDX12::LogAdapters()
{
    UINT i = 0;
    IDXGIAdapter* adapter = nullptr;
    std::vector<IDXGIAdapter*> adapterList;
    while (mdxgiFactory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND)
    {
        DXGI_ADAPTER_DESC desc;
        adapter->GetDesc(&desc);

        std::wstring text = L"***Adapter: ";
        text += desc.Description;
        text += L"\n";

        OutputDebugString(text.c_str());

        adapterList.push_back(adapter);

        ++i;
    }

    for (size_t i = 0; i < adapterList.size(); ++i)
    {
        LogAdapterOutput(adapterList[i]);
        ReleaseCom(adapterList[i]);
    }
}

#endif


void RenderDeviceDX12::CreateCommandObjects(const RenderDeviceInitParams& initParams)
{
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    ThrowIfFailed(md3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&mCommandQueue)));

    ThrowIfFailed(md3dDevice->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        IID_PPV_ARGS(mDirectCmdListAlloc.GetAddressOf())));

    ThrowIfFailed(md3dDevice->CreateCommandList(
        0,
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        mDirectCmdListAlloc.Get(), // Associated command allocator
        nullptr, // Initial PipelineStateObject
        IID_PPV_ARGS(mCommandList.GetAddressOf())));

    // Start off in a closed state.  This is because the first time we refer 
    // to the command list we will Reset it, and it needs to be closed before
    // calling Reset.
    mCommandList->Close();
}

void RenderDeviceDX12::CreateSwapChain(const RenderDeviceInitParams& initParams)
{
    // Release the previous swapchain we will be recreating.
    mSwapChain.Reset();

    DXGI_SWAP_CHAIN_DESC sd;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferDesc.Format = mBackBufferFormat;
    sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount = SwapChainBufferCount;
    sd.OutputWindow = (HWND)initParams.deviceWindowHandle;
    sd.Windowed = true;
    sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    // Note: Swap chain uses queue to perform flush.
    ThrowIfFailed(mdxgiFactory->CreateSwapChain(
        mCommandQueue.Get(),
        &sd,
        mSwapChain.GetAddressOf()));
}

void RenderDeviceDX12::CreateRtvAndDsvDescriptorHeaps()
{
    constexpr auto CreateDH = [this]
    (
        D3D12_DESCRIPTOR_HEAP_TYPE Type,
        UINT NumDescriptors,
        D3D12_DESCRIPTOR_HEAP_FLAGS Flags,
        UINT NodeMask,
        DirectX::DescriptorHeap*& ptr
    )-> void
    {
        ID3D12DescriptorHeap* heapPtr;
        D3D12_DESCRIPTOR_HEAP_DESC heapDesc;
        heapDesc.NumDescriptors = NumDescriptors;
        heapDesc.Type = Type;
        heapDesc.Flags = Flags;
        heapDesc.NodeMask = NodeMask;
        ThrowIfFailed(md3dDevice->CreateDescriptorHeap(
            &heapDesc,
            IID_PPV_ARGS(&heapPtr)));
        ptr = new DirectX::DescriptorHeap(heapPtr);
    };

    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
    rtvHeapDesc.NumDescriptors = SwapChainBufferCount;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    rtvHeapDesc.NodeMask = 0;
    ThrowIfFailed(md3dDevice->CreateDescriptorHeap(
        &rtvHeapDesc, IID_PPV_ARGS(mBBHeap.GetAddressOf())));


    CreateDH(
        D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
        DX12Limitations::maxDSV,
        D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
        0,
        mDsvHeapInterface
    );

    CreateDH(
        D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
        DX12Limitations::maxRTV,
        D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
        0,
        mRTVHeapInterface
    );

    CreateDH(
        D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER,
        DX12Limitations::maxSamplers,
        D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
        0,
        mSamplerHeapInterface
    );

    CreateDH(
        D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
        DX12Limitations::maxTextures,
        D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
        0,
        mShvHeapInterface
    );

    CreateDH(
        D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
        DX12Limitations::maxConstBuffers,
        D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
        0,
        mCbHeapInterface
    );

    CreateDH(
        D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
        DX12Limitations::maUA,
        D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
        0,
        mUaHeapInterface
    );


    D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    CD3DX12_ROOT_PARAMETER rootParameters[RootParameterIndex::RootParameterCount] = {};

    CD3DX12_DESCRIPTOR_RANGE uaRange(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, DX12Limitations::maUA, 0);
    rootParameters[RootParameterIndex::UABase].InitAsDescriptorTable(1, &uaRange, D3D12_SHADER_VISIBILITY_ALL);

    CD3DX12_DESCRIPTOR_RANGE cbRange(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, DX12Limitations::maxConstBuffers, 0);
    rootParameters[RootParameterIndex::CBBase].InitAsDescriptorTable(0, 0, D3D12_SHADER_VISIBILITY_ALL);

    CD3DX12_DESCRIPTOR_RANGE textureRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, DX12Limitations::maxTextures, 0);
    rootParameters[RootParameterIndex::SRVBase].InitAsDescriptorTable(1, &textureRange, D3D12_SHADER_VISIBILITY_ALL);

    CD3DX12_DESCRIPTOR_RANGE textureSamplerRange(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, DX12Limitations::maxSamplers, 0);
    rootParameters[RootParameterIndex::SamplerBase].InitAsDescriptorTable(
        1, &textureSamplerRange, D3D12_SHADER_VISIBILITY_ALL);

    // Create the root signature
    CD3DX12_ROOT_SIGNATURE_DESC rsigDesc = {};
    rsigDesc.Init(_countof(rootParameters), rootParameters, 0, nullptr, rootSignatureFlags);


    ComPtr<ID3DBlob> pSignature;
    ComPtr<ID3DBlob> pError;
    HRESULT hr = D3D12SerializeRootSignature(&rsigDesc, D3D_ROOT_SIGNATURE_VERSION_1, pSignature.GetAddressOf(),
                                             pError.GetAddressOf());
    if (SUCCEEDED(hr))
    {
        hr = device->CreateRootSignature(0, pSignature->GetBufferPointer(), pSignature->GetBufferSize(),
                                         IID_PPV_ARGS(&rootSignature)
        );
    }


    CreateDH(
        D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
        1000,
        D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
        0,
        srvStorageHeap
    );
    CreateDH(
        D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
        1000,
        D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
        0,
        rtvStorageHeap
    );
    CreateDH(
        D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER,
        1000,
        D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
        0,
        samplersStorageHeap
    );
}

D3D12_CPU_DESCRIPTOR_HANDLE RenderDeviceDX12::CurrentBackBufferView() const
{
    // CD3DX12 constructor to offset to the RTV of the current back buffer.
    return CD3DX12_CPU_DESCRIPTOR_HANDLE(
        mBBHeap->GetCPUDescriptorHandleForHeapStart(), // handle start
        mCurrBackBuffer, // index to offset
        mRtvDescriptorSize); // byte size of descriptor
}

IRenderDevice::IResource* RenderDeviceDX12::CreateResource(const GpuResource& ResourceDesc)
{
    switch (ResourceDesc.resourceDescription.Dimension)
    {
        case EResourceDimension::RESOURCE_DIMENSION_BUFFER:
        {
            ID3D12Resource* defaultBuffer;

            // Create the actual default buffer resource.
            ThrowIfFailed(device->CreateCommittedResource(
                &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
                D3D12_HEAP_FLAG_NONE,
                &CD3DX12_RESOURCE_DESC::Buffer(ResourceDesc.resourceDescription.Width),
                D3D12_RESOURCE_STATE_COMMON,
                nullptr,
                IID_PPV_ARGS(&defaultBuffer)));

            return (IRenderDevice::IResource*)defaultBuffer;
        }
        case EResourceDimension::RESOURCE_DIMENSION_TEXTURE1D:
        {
            ID3D12Resource* defaultBuffer;

            // Create the actual default buffer resource.
            ThrowIfFailed(device->CreateCommittedResource(
                &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
                D3D12_HEAP_FLAG_NONE,
                &CD3DX12_RESOURCE_DESC::Tex1D(
                    ToD3D_TextureFormat[to_underlying(ResourceDesc.resourceDescription.Format)],
                    ResourceDesc.resourceDescription.Width,
                    ResourceDesc.resourceDescription.DepthOrArraySize
                ),
                D3D12_RESOURCE_STATE_COMMON,
                nullptr,
                IID_PPV_ARGS(&defaultBuffer)));

            return (IRenderDevice::IResource*)defaultBuffer;
        }
        case EResourceDimension::RESOURCE_DIMENSION_TEXTURE2D:
        {
            ID3D12Resource* defaultBuffer;

            // Create the actual default buffer resource.
            ThrowIfFailed(device->CreateCommittedResource(
                &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
                D3D12_HEAP_FLAG_NONE,
                &CD3DX12_RESOURCE_DESC::Tex2D(
                    ToD3D_TextureFormat[to_underlying(ResourceDesc.resourceDescription.Format)],
                    ResourceDesc.resourceDescription.Width,
                    ResourceDesc.resourceDescription.Height,
                    ResourceDesc.resourceDescription.DepthOrArraySize
                ),
                D3D12_RESOURCE_STATE_COMMON,
                nullptr,
                IID_PPV_ARGS(&defaultBuffer)));

            return (IRenderDevice::IResource*)defaultBuffer;
        }
        case EResourceDimension::RESOURCE_DIMENSION_TEXTURE3D:
        {
            ID3D12Resource* defaultBuffer;

            // Create the actual default buffer resource.
            ThrowIfFailed(device->CreateCommittedResource(
                &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
                D3D12_HEAP_FLAG_NONE,
                &CD3DX12_RESOURCE_DESC::Tex3D(
                    ToD3D_TextureFormat[to_underlying(ResourceDesc.resourceDescription.Format)],
                    ResourceDesc.resourceDescription.Width,
                    ResourceDesc.resourceDescription.Height,
                    ResourceDesc.resourceDescription.DepthOrArraySize
                ),
                D3D12_RESOURCE_STATE_COMMON,
                nullptr,
                IID_PPV_ARGS(&defaultBuffer)));

            return (IRenderDevice::IResource*)defaultBuffer;
        }
    }
}

IRenderDevice::INPUTLAYOUTHANDLE RenderDeviceDX12::CreateInputLayout(const InputAssemblerDeclarationDesc& desc,
    const ShaderDesc& Shader)
{
    
}

RenderDeviceDX12::RESOURCEVIEWHANDLE RenderDeviceDX12::CreateResourceView(
    const GpuResourceView& ViewDesc, const GpuResource& ResourceDesc)
{
    RESOURCEVIEWHANDLE output;

    ID3D12Resource* resource = (ID3D12Resource*)ResourceDesc.resource;
    switch (ViewDesc.type)
    {
        case GpuResourceView::EViewType::CB:
        {
            D3D12_CONSTANT_BUFFER_VIEW_DESC desc = {};
            desc.BufferLocation = resource->GetGPUVirtualAddress() + ViewDesc.cbViewDescription.Offset;
            desc.SizeInBytes = ViewDesc.cbViewDescription.Size;

            auto descriptorHandle = srvStorageHeap->GetNextCpuHandle();
            device->CreateConstantBufferView(
                &desc,
                descriptorHandle
            );

            output.data = descriptorHandle.ptr;
        }
        case GpuResourceView::EViewType::IB:
        {
            D3D12_INDEX_BUFFER_VIEW desc = {};
            desc.BufferLocation = resource->GetGPUVirtualAddress() + ViewDesc.ibViewDescription.Offset;
            switch (ViewDesc.ibViewDescription.Format)
            {
                case EFormat::FORMAT_R16_UINT:
                    desc.Format = DXGI_FORMAT_R16_UINT;
                    break;
                case EFormat::FORMAT_R32_UINT:
                    desc.Format = DXGI_FORMAT_R32_UINT;
                    break;
            }
            desc.SizeInBytes = ViewDesc.ibViewDescription.Size;

            indexBuffers.push_back(desc);

            output.data = indexBuffers.size();
        }
        case GpuResourceView::EViewType::VB:
        {
            const auto& view = ViewDesc.vbViewDescription;
            D3D12_VERTEX_BUFFER_VIEW desc = {};
            desc.BufferLocation = resource->GetGPUVirtualAddress() + view.Offset;
            desc.SizeInBytes = view.Size;
            desc.StrideInBytes = view.Stride;

            vertexBuffers.push_back(desc);

            output.data = vertexBuffers.size();
        }
        case GpuResourceView::EViewType::DB:
        {
            D3D12_DEPTH_STENCIL_VIEW_DESC desc = {};
            const auto& view = ViewDesc.dbViewDescription;
            desc= ToD3D12DepthStencilView(view);

            auto descriptorHandle = srvStorageHeap->GetNextCpuHandle();
            device->CreateDepthStencilView(
                resource,
                &desc,
                descriptorHandle
            );
            
            descriptorFormats.insert({descriptorHandle.ptr,desc.Format});

            output.data = descriptorHandle.ptr;
        }
        case GpuResourceView::EViewType::RT:
        {
            D3D12_RENDER_TARGET_VIEW_DESC desc = {};
            const auto& view = ViewDesc.rtViewDescription;
            desc = ToD3D12RTView(view);

            auto descriptorHandle = srvStorageHeap->GetNextCpuHandle();
            device->CreateRenderTargetView(
                resource,
                &desc,
                descriptorHandle
            );

            descriptorFormats.insert({descriptorHandle.ptr,desc.Format});

            output.data = descriptorHandle.ptr;
        }
        case GpuResourceView::EViewType::SR:
        {
            D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
            const auto& view = ViewDesc.srViewDescription;
            desc = ToD3D12ShaderView(view);

            auto descriptorHandle = srvStorageHeap->GetNextCpuHandle();
            device->CreateShaderResourceView(
                resource,
                &desc,
                descriptorHandle
            );

            output.data = descriptorHandle.ptr;
        }
        case GpuResourceView::EViewType::UA:
        {
            D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
            const auto& view = ViewDesc.uaViewDescription;
            desc = ToD3D12UAView(view);

            auto descriptorHandle = srvStorageHeap->GetNextCpuHandle();
            device->CreateUnorderedAccessView(
                resource,
                nullptr,
                &desc,
                descriptorHandle
            );
            
            descriptorFormats.insert({descriptorHandle.ptr,desc.Format});
            output.data = descriptorHandle.ptr;
        }
    }
    return output;
}
