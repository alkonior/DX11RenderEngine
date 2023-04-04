// ReSharper disable CppMsExtAddressOfClassRValue
#include "RenderDeviceDX12.h"
#include "../DX12Helpers/d3dUtil.h"
#include "GPUResourcesDescription/GPUResource.h"
#include "GVMToDx12.h"
#include "DescriptorHeap.h"

template<class T>
using ComPtr = wrl::ComPtr<T>;
using namespace GVM;

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib,"d3d12.lib")


enum DX12Limitations {
    maxDSV = 1,
    maxRTV = 8,
    maxUA = 8,
    maxTextures = 40,
    maxVertexBuffers = 10,
    maxConstBuffers = 16,
    maxSamplers = 20,
};


RenderDeviceDX12::RenderDeviceDX12(const RenderDeviceInitParams& initParams, bool debugMode) :
    IRenderDevice(initParams, debugMode)
{
#if defined(DEBUG) || defined(_DEBUG)
    // Enable the D3D12 debug layer.
    {
        ComPtr<ID3D12Debug> debugController;
        ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
        debugController->EnableDebugLayer();
    }
#endif

    UINT createFactoryFlags = 0;
#if defined(_DEBUG)
    createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

    ThrowIfFailed(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&mdxgiFactory)));

    // Try to create hardware device.
    HRESULT hardwareResult = D3D12CreateDevice(
        nullptr, // default adapter
        D3D_FEATURE_LEVEL_12_0,
        IID_PPV_ARGS(&md3dDevice));

    // Fallback to WARP device.
    if (FAILED(hardwareResult))
    {
        ComPtr<IDXGIAdapter> pWarpAdapter;
        ThrowIfFailed(mdxgiFactory->EnumAdapters(0, &pWarpAdapter));

        ThrowIfFailed(D3D12CreateDevice(
            pWarpAdapter.Get(),
            D3D_FEATURE_LEVEL_11_0,
            IID_PPV_ARGS(&md3dDevice)));
    }
#if defined(_DEBUG)
    ComPtr<ID3D12InfoQueue> pInfoQueue;
    if (SUCCEEDED(md3dDevice.As(&pInfoQueue)))
    {
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, FALSE);
        // Suppress whole categories of messages
        //D3D12_MESSAGE_CATEGORY Categories[] = {};

        // Suppress messages based on their severity level
        D3D12_MESSAGE_SEVERITY Severities[] =
        {
            D3D12_MESSAGE_SEVERITY_INFO
        };

        // Suppress individual messages by their ID
        D3D12_MESSAGE_ID DenyIds[] = {
            D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,
            // I'm really not sure how to avoid this message.
            D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
            // This warning occurs when using capture frame while graphics debugging.
            D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,
            // This warning occurs when using capture frame while graphics debugging.
            D3D12_MESSAGE_ID_SET_INDEX_BUFFER_INVALID,
            // This warning occurs when no index buffer.
            D3D12_MESSAGE_ID_CLEARDEPTHSTENCILVIEW_MISMATCHINGCLEARVALUE,
            D3D12_MESSAGE_ID_COMMAND_LIST_DRAW_INDEX_BUFFER_NOT_SET,
            D3D12_MESSAGE_ID_RESOURCE_BARRIER_MATCHING_STATES

        };

        D3D12_INFO_QUEUE_FILTER NewFilter = {};
        //NewFilter.DenyList.NumCategories = _countof(Categories);
        //NewFilter.DenyList.pCategoryList = Categories;
        NewFilter.DenyList.NumSeverities = _countof(Severities);
        NewFilter.DenyList.pSeverityList = Severities;
        NewFilter.DenyList.NumIDs = _countof(DenyIds);
        NewFilter.DenyList.pIDList = DenyIds;

        ThrowIfFailed(pInfoQueue->PushStorageFilter(&NewFilter));

        //ComPtr<ID3D12Debug> spDebugController0;
        //ComPtr<ID3D12Debug1> spDebugController1;
        //ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&spDebugController0)));
        //ThrowIfFailed(spDebugController0->QueryInterface(IID_PPV_ARGS(&spDebugController1)));
        //spDebugController1->SetEnableGPUBasedValidation(true);
    }
#endif

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

    vertexBuffersPtr.resize(DX12Limitations::maxVertexBuffers);
    try
    {
        CreateCommandObjects(initParams);
        CreateSwapChain(initParams);
        CreateRtvAndDsvDescriptorHeaps();

        currentBufferUploadBufferSize = 10000;
        for(int i = 0; i < SwapChainBufferCount; i++)
        {
            ThrowIfFailed(md3dDevice->CreateCommittedResource(
                &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
                D3D12_HEAP_FLAG_NONE,
                &CD3DX12_RESOURCE_DESC::Buffer( currentBufferUploadBufferSize ),
                D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(&currentBufferUploadBuffer)));
            
            Frames[i].uploadBuffer = currentBufferUploadBuffer;
            Frames[i].currentBufferUploadBufferSize = currentBufferUploadBufferSize;
        }
        
        Frames[mCurrBackBuffer].uploadBuffers.clear();
        Frames[mCurrBackBuffer].uploadTextureBuffers.clear();
        currentBufferUploadBuffer =  Frames[mCurrBackBuffer].uploadBuffer;
        mCommandList =  Frames[mCurrBackBuffer].mCommandList;
        mDirectCmdListAlloc =  Frames[mCurrBackBuffer].mDirectCmdListAlloc;
        mGpuSamplerHeapInterface =  Frames[mCurrBackBuffer].mGpuSamplerHeapInterface;
        mGpuShvCbUaHeapInterface =  Frames[mCurrBackBuffer].mGpuShvCbUaHeapInterface;
        bufferUploadBuffers.push_back(currentBufferUploadBuffer);
        
        ThrowIfFailed(currentBufferUploadBuffer->Map(0, NULL, reinterpret_cast<void**>(&currentBufferUploadBufferPtr)));
        
        mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
            D3D12_RESOURCE_STATE_PRESENT,
            D3D12_RESOURCE_STATE_RENDER_TARGET));

        //CreateDefaultHandles();
        ThrowIfFailed(md3dDevice->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(CalcConstantBufferByteSize(256)),
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&cbResource)));

        D3D12_CONSTANT_BUFFER_VIEW_DESC desc = {};
        desc.BufferLocation = cbResource->GetGPUVirtualAddress();
        desc.SizeInBytes = 256;

        defaultCB = srvStorageHeap->GetNextCpuHandle();
        md3dDevice->CreateConstantBufferView(
            &desc,
            defaultCB
        );


        ThrowIfFailed(md3dDevice->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM,256,256),
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&shResource)));


        defaultTexture = srvStorageHeap->GetNextCpuHandle();
        md3dDevice->CreateShaderResourceView(
            shResource.Get(),
            NULL,
            defaultTexture
        );

        GVM::SamplerStateDesc sd = {
        };
        sd.Filter = ESamplerFilter::FILTER_ANISOTROPIC;
        sd.AddressU = ETextureAddressMode::TEXTURE_ADDRESS_WRAP;
        sd.AddressV = ETextureAddressMode::TEXTURE_ADDRESS_WRAP;
        sd.AddressW = ETextureAddressMode::TEXTURE_ADDRESS_WRAP;
        defaultSampler = FetchSamplerState(
            sd
        );
    }
    catch (std::exception& ex)
    {
        throw ex;
    }
}


RenderDeviceDX12::~RenderDeviceDX12()
{}

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
    UINT ii = 0;
    IDXGIAdapter* adapter = nullptr;
    std::vector<IDXGIAdapter*> adapterList;
    while (mdxgiFactory->EnumAdapters(ii, &adapter) != DXGI_ERROR_NOT_FOUND)
    {
        DXGI_ADAPTER_DESC desc;
        adapter->GetDesc(&desc);

        std::wstring text = L"***Adapter: ";
        text += desc.Description;
        text += L"\n";

        OutputDebugString(text.c_str());

        adapterList.push_back(adapter);

        ++ii;
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
    for (int i =0 ; i < SwapChainBufferCount ; i++)
    {
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

        // Wait until frame commands are complete.  This waiting is inefficient and is
        // done for simplicity.  Later we will show how to organize our rendering code
        // so we do not have to wait per frame.
        FlushCommandQueue();

        if (i == 0)
        {
            // Reuse the memory associated with command recording.
            // We can only reset when the associated command lists have finished execution on the GPU.
            ThrowIfFailed(mDirectCmdListAlloc->Reset());

            // A command list can be reset after it has been added to the command queue via ExecuteCommandList.
            // Reusing the command list reuses memory.
            ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));
        }

        Frames[i].mDirectCmdListAlloc = mDirectCmdListAlloc;
        Frames[i].mCommandList = mCommandList;
        Frames[i].fenceValue = mCurrentFence;
    }
    mCommandList = Frames[0].mCommandList;
    mDirectCmdListAlloc = Frames[0].mDirectCmdListAlloc;
}

void RenderDeviceDX12::CreateSwapChain(const RenderDeviceInitParams& initParams)
{
    // Release the previous swapchain we will be recreating.
    mSwapChain.Reset();
    BackBufferWidth = initParams.backBufferWidth;
    BackBufferHeight = initParams.backBufferHeight;

    DXGI_SWAP_CHAIN_DESC sd;
    sd.BufferDesc.Width = initParams.backBufferWidth;
    sd.BufferDesc.Height = initParams.backBufferHeight;
    sd.BufferDesc.RefreshRate.Numerator = 0;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferDesc.Format = mBackBufferFormat;
    sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.BufferUsage = DXGI_USAGE_BACK_BUFFER;
    sd.BufferCount = SwapChainBufferCount;
    sd.OutputWindow = ((GVM::Dx12PlatformHandle*)initParams.deviceWindowHandle)->hwnd;
    sd.Windowed = true;
    sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    sd.Flags = 0;

    // Note: Swap chain uses queue to perform flush.


    ThrowIfFailed(mdxgiFactory->CreateSwapChain(
        mCommandQueue.Get(),
        &sd,
        &mSwapChain));
}

ID3D12Resource* RenderDeviceDX12::CurrentBackBuffer() const
{
    return mSwapChainBuffer[mCurrBackBuffer].Get();
}

D3D12_CPU_DESCRIPTOR_HANDLE RenderDeviceDX12::CurrentBackBufferView()
{
    return CD3DX12_CPU_DESCRIPTOR_HANDLE(
        mBBHeap->GetCPUDescriptorHandleForHeapStart(),
        mCurrBackBuffer,
        mRtvDescriptorSize);
}

void RenderDeviceDX12::CreateRtvAndDsvDescriptorHeaps()
{
    const auto CreateDH = [this]
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

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(mBBHeap->GetCPUDescriptorHandleForHeapStart());
    for (UINT i = 0; i < SwapChainBufferCount; i++)
    {
        ThrowIfFailed(mSwapChain->GetBuffer(i, IID_PPV_ARGS(&mSwapChainBuffer[i])));

        //mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mSwapChainBuffer[i].Get(),
        //    D3D12_RESOURCE_STATE_COMMON , D3D12_RESOURCE_STATE_PRESENT));
        md3dDevice->CreateRenderTargetView(mSwapChainBuffer[i].Get(), nullptr, rtvHeapHandle);
        rtvHeapHandle.Offset(1, mRtvDescriptorSize);

        CreateDH(
            D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER,
            2048,
            D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
            0,
            mGpuSamplerHeapInterface
        );

        CreateDH(
            D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
            (DX12Limitations::maxUA + DX12Limitations::maxTextures + DX12Limitations::maxConstBuffers) * 1000,
            D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
            0,
            mGpuShvCbUaHeapInterface
        );
        
        Frames[i].mGpuSamplerHeapInterface = mGpuSamplerHeapInterface;
        Frames[i].mGpuShvCbUaHeapInterface = mGpuShvCbUaHeapInterface;
    }


    //GPU Descriptor heaps


    ID3D12DescriptorHeap* ppHeaps[] = {mGpuShvCbUaHeapInterface->Heap(),mGpuSamplerHeapInterface->Heap()};
    mCommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
    //

    CreateDH(
        D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
        DX12Limitations::maxDSV * 1000,
        D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
        0,
        mDsvHeapInterface
    );
    CreateDH(
        D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
        DX12Limitations::maxRTV * 1000,
        D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
        0,
        mRtvHeapInterface
    );
    CreateDH(
        D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
        10000,
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


IRenderDevice::IResource* RenderDeviceDX12::CreateResource(const GpuResource& ResourceDesc)
{
    ID3D12Resource* defaultBuffer = nullptr;
    switch (ResourceDesc.resourceDescription.Dimension)
    {
    case EResourceDimension::RESOURCE_DIMENSION_BUFFER:
    {
        // Create the actual default buffer resource.
        ThrowIfFailed(md3dDevice->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(CalcConstantBufferByteSize(ResourceDesc.resourceDescription.Width)),
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&defaultBuffer)));
        break;
    }
    case EResourceDimension::RESOURCE_DIMENSION_TEXTURE1D:
    {
        //ID3D12Resource* defaultBuffer;

        // Create the actual default buffer resource.
        ThrowIfFailed(md3dDevice->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Tex1D(
                ToD3D_TextureFormat[to_underlying(ResourceDesc.resourceDescription.Format)],
                ResourceDesc.resourceDescription.Width,
                ResourceDesc.resourceDescription.DepthOrArraySize,
                1,
                ToDx12TextureFlags(ResourceDesc.resourceBindings)
            ),
            D3D12_RESOURCE_STATE_GENERIC_READ,
            CreateClearValue(ToD3D_TextureFormat[to_underlying(ResourceDesc.resourceDescription.Format)],
                ResourceDesc.resourceBindings),
            IID_PPV_ARGS(&defaultBuffer)));
        break;
    }
    case EResourceDimension::RESOURCE_DIMENSION_TEXTURE2D:
    {
        //ID3D12Resource* defaultBuffer;

        // Create the actual default buffer resource.
        ThrowIfFailed(md3dDevice->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Tex2D(
                ToD3D_TextureFormat[to_underlying(ResourceDesc.resourceDescription.Format)],
                ResourceDesc.resourceDescription.Width,
                ResourceDesc.resourceDescription.Height,
                ResourceDesc.resourceDescription.DepthOrArraySize,
                1,
                1,
                0,
                ToDx12TextureFlags(ResourceDesc.resourceBindings)
            ),
            D3D12_RESOURCE_STATE_GENERIC_READ,
            CreateClearValue(ToD3D_TextureFormat[to_underlying(ResourceDesc.resourceDescription.Format)],
                ResourceDesc.resourceBindings),
            IID_PPV_ARGS(&defaultBuffer)));
        break;
    }
    case EResourceDimension::RESOURCE_DIMENSION_TEXTURE3D:
    {
        //ID3D12Resource* defaultBuffer;

        // Create the actual default buffer resource.
        ThrowIfFailed(md3dDevice->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Tex3D(
                ToD3D_TextureFormat[to_underlying(ResourceDesc.resourceDescription.Format)],
                ResourceDesc.resourceDescription.Width,
                ResourceDesc.resourceDescription.Height,
                ResourceDesc.resourceDescription.DepthOrArraySize,
                1,
                ToDx12TextureFlags(ResourceDesc.resourceBindings)
            ),
            D3D12_RESOURCE_STATE_GENERIC_READ,
            CreateClearValue(ToD3D_TextureFormat[to_underlying(ResourceDesc.resourceDescription.Format)],
                ResourceDesc.resourceBindings),
            IID_PPV_ARGS(&defaultBuffer)));
        break;
    }
    default: break;
    }
    Resources.resize((uintptr_t)ResourceDesc.id + 1);
    Resources[(uintptr_t)ResourceDesc.id] = defaultBuffer;
    if (defaultBuffer == nullptr)
    {
        defaultBuffer = nullptr;
    }
    return (IRenderDevice::IResource*)defaultBuffer;
}

enum RootParameterIndex {
    UA_SRVBase_CBBase,
    SamplerBase,
    RootParameterCount
};


ID3D12RootSignature* RenderDeviceDX12::FetchRS(SignatureParams params)
{
    ID3D12RootSignature* rs = nullptr;
    auto it = RootSignatures.begin();
    if ((it = std::find_if(RootSignatures.begin(), RootSignatures.end(),
            [params](const std::pair<uint32_t, wrl::ComPtr<ID3D12RootSignature>>& i)
            {
                return i.first == params.data;
            }))
        != RootSignatures.end())
    {
        rs = it->second.Get();
    }
    else
    {
        Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
        D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

        CD3DX12_ROOT_PARAMETER rootParameters[2] = {};


        std::vector<CD3DX12_DESCRIPTOR_RANGE> descriptorRanges;
        CD3DX12_DESCRIPTOR_RANGE textureSamplerRange;
        if (params.uaCount)
        {
            descriptorRanges.push_back(CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, params.uaCount, 0));
        };
        if (params.cbCount)
        {
            descriptorRanges.push_back(CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, params.cbCount, 0));
        }
        if (params.srvCount)
        {
            descriptorRanges.push_back(CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, params.srvCount, 0));
        }
        if (descriptorRanges.size())
        {
            rootParameters[RootParameterIndex::UA_SRVBase_CBBase].InitAsDescriptorTable(
                descriptorRanges.size(), descriptorRanges.data(), D3D12_SHADER_VISIBILITY_ALL);
        }
        if (params.samplersCount)
        {
            textureSamplerRange = CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER,
                DX12Limitations::maxSamplers, 0);
            rootParameters[RootParameterIndex::SamplerBase].InitAsDescriptorTable(
                1, &textureSamplerRange, D3D12_SHADER_VISIBILITY_ALL);
        }


        CD3DX12_ROOT_SIGNATURE_DESC rsigDesc = {};
        auto paramsEnum = (descriptorRanges.size() > 0) + (params.samplersCount > 0) * 2;
        switch (paramsEnum)
        {
        case 0: //NO params at all
        {
            rsigDesc.Init(0, nullptr, 0, nullptr, rootSignatureFlags);
            break;
        };
        case 1: //NO sampler
        {
            rsigDesc.Init(1, rootParameters, 0, nullptr, rootSignatureFlags);
            break;
        };
        case 2: //NO cb,ua,shv
        {
            rsigDesc.Init(1, rootParameters + 1, 0, nullptr, rootSignatureFlags);
            break;
        };
        case 3: // samplers+all...
        {
            rsigDesc.Init(2, rootParameters, 0, nullptr, rootSignatureFlags);
            break;
        }
        default:
        {
            throw std::exception("Something very bad!!!");
        }
        }

        ComPtr<ID3DBlob> pSignature;
        ComPtr<ID3DBlob> pError;
#if _DEBUG
        try
        {
            ThrowIfFailed(
                D3D12SerializeRootSignature(&rsigDesc, D3D_ROOT_SIGNATURE_VERSION_1, pSignature.GetAddressOf(),
                    pError.GetAddressOf()));
        }
        catch (std::exception& ex)
        {
            GVM::CompileException ce{
                __LINE__, __FILE__,(hr),infoManager.GetMessages(),(char*)pError->GetBufferPointer(),
                "Root Signature."
            };
            throw ce;
        }
#else
        HRESULT hr__ =
            D3D12SerializeRootSignature(&rsigDesc, D3D_ROOT_SIGNATURE_VERSION_1, pSignature.GetAddressOf(),
                pError.GetAddressOf());
        if (FAILED(hr__)) {
            auto* error =
                (char*)pError->GetBufferPointer();
            std::wcout << error << std::endl;
            throw GVM::HrException(__LINE__, __FILE__, hr__);
        }
#endif
        ThrowIfFailed(md3dDevice->CreateRootSignature(0, pSignature->GetBufferPointer(), pSignature->GetBufferSize(),
            IID_PPV_ARGS(&rootSignature)));
        RootSignatures.push_back({params.data,rootSignature});

        std::wstring psName = L"RootSignature_";
        psName += std::wstring(L"ua") + std::to_wstring(params.uaCount) + std::wstring(L"_").c_str();
        psName += std::wstring(L"cb") + std::to_wstring(params.cbCount) + std::wstring(L"_").c_str();
        psName += std::wstring(L"srv") + std::to_wstring(params.srvCount) + std::wstring(L"_").c_str();
        psName += std::wstring(L"sam") + std::to_wstring(params.samplersCount) + std::wstring(L"_").c_str();
        rootSignature->SetName(psName.c_str());
        rs = rootSignature.Get();
    }

    return rs;
}

void RenderDeviceDX12::DestroyResource(RESOURCEHANDLE const resource)
{
    ID3D12Resource* resourcePtr = (ID3D12Resource*)resource;
    if (resourcePtr)
        resourcePtr->Release();
}

void RenderDeviceDX12::DestroyResourceView(const RESOURCEVIEWHANDLE resource)
{}

IRenderDevice::SHADERHANDLE RenderDeviceDX12::CreateShader(const ShaderDesc& desc)
{
    shaders.push_back({desc.bytecode,desc.byteCodeSize});
    return (IRenderDevice::SHADERHANDLE)shaders.size();
}

IRenderDevice::INPUTLAYOUTHANDLE RenderDeviceDX12::CreateInputLayout(const InputAssemblerDeclarationDesc& desc,
    const ShaderDesc& Shader)
{
    inputLayoutsData.push_back(ToD3D11(desc));
    D3D12_INPUT_LAYOUT_DESC inp;
    inp.NumElements = inputLayoutsData.rbegin()->size();
    inp.pInputElementDescs = inputLayoutsData.rbegin()->data();
    inputLayouts.push_back(inp);
    return (IRenderDevice::INPUTLAYOUTHANDLE)inputLayouts.size();
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
        desc.SizeInBytes = CalcConstantBufferByteSize(ViewDesc.cbViewDescription.Size);

        auto descriptorHandle = srvStorageHeap->GetNextCpuHandle();
        md3dDevice->CreateConstantBufferView(
            &desc,
            descriptorHandle
        );

        output.data = descriptorHandle.ptr;
        break;
    }
    case GpuResourceView::EViewType::IB:
    {
        D3D12_INDEX_BUFFER_VIEW desc = {};
        desc.BufferLocation = resource->GetGPUVirtualAddress() + ViewDesc.ibViewDescription.Offset;
        switch (ViewDesc.ibViewDescription.Format)
        {
        case EFormat::FORMAT_R16_UINT: desc.Format = DXGI_FORMAT_R16_UINT;
            break;
        case EFormat::FORMAT_R32_UINT: desc.Format = DXGI_FORMAT_R32_UINT;
            break;
        }
        desc.SizeInBytes = resource->GetDesc().Width;

        indexBuffers.push_back(desc);

        output.data = indexBuffers.size();
        break;
    }
    case GpuResourceView::EViewType::VB:
    {
        const auto& view = ViewDesc.vbViewDescription;
        D3D12_VERTEX_BUFFER_VIEW desc = {};
        desc.BufferLocation = resource->GetGPUVirtualAddress() + view.Offset;
        desc.SizeInBytes = resource->GetDesc().Width;
        desc.StrideInBytes = view.Stride;

        vertexBuffers.push_back(desc);

        output.data = vertexBuffers.size();
        break;
    }
    case GpuResourceView::EViewType::DB:
    {
        D3D12_DEPTH_STENCIL_VIEW_DESC desc = {};
        const auto& view = ViewDesc.dbViewDescription;
        desc = ToD3D12DepthStencilView(view);

        auto descriptorHandle = mDsvHeapInterface->GetNextCpuHandle();
        md3dDevice->CreateDepthStencilView(
            resource,
            &desc,
            descriptorHandle
        );

        //descriptorFormats.insert({descriptorHandle.ptr, desc.Format});

        output.data = descriptorHandle.ptr;
        break;
    }
    case GpuResourceView::EViewType::RT:
    {
        D3D12_RENDER_TARGET_VIEW_DESC desc = {};
        const auto& view = ViewDesc.rtViewDescription;
        desc = ToD3D12RTView(view);

        auto descriptorHandle = rtvStorageHeap->GetNextCpuHandle();
        if (!view.MakeDefault)
            md3dDevice->CreateRenderTargetView(
                resource,
                &desc,
                descriptorHandle
            );
        else
            md3dDevice->CreateRenderTargetView(
                resource,
                NULL,
                descriptorHandle
            );

        //descriptorFormats.insert({descriptorHandle.ptr, desc.Format});

        output.data = descriptorHandle.ptr;
        break;
    }
    case GpuResourceView::EViewType::SR:
    {
        D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
        const auto& view = ViewDesc.srViewDescription;
        desc = ToD3D12ShaderView(view);

        auto descriptorHandle = srvStorageHeap->GetNextCpuHandle();
        if (!view.MakeDefault)
            md3dDevice->CreateShaderResourceView(
                resource,
                &desc,
                descriptorHandle
            );
        else
            md3dDevice->CreateShaderResourceView(
                resource,
                NULL,
                descriptorHandle
            );

        output.data = descriptorHandle.ptr;
        break;
    }
    case GpuResourceView::EViewType::UA:
    {
        D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
        const auto& view = ViewDesc.uaViewDescription;
        desc = ToD3D12UAView(view);
        auto descriptorHandle = srvStorageHeap->GetNextCpuHandle();
        if (!view.MakeDefault)
            md3dDevice->CreateUnorderedAccessView(
                resource,
                nullptr,
                &desc,
                descriptorHandle
            );
        else
        {
            md3dDevice->CreateUnorderedAccessView(
                resource,
                nullptr,
                NULL,
                descriptorHandle
            );
        }

        //descriptorFormats.insert({descriptorHandle.ptr, desc.Format});
        output.data = descriptorHandle.ptr;
        break;
    }
    }
    return output;
}

void* RenderDeviceDX12::GetNativeTexture(RESOURCEVIEWHANDLE view)
{
    return nullptr;
}


void UploadTextureData(
    ID3D12Device* device,
    ID3D12GraphicsCommandList* cmdList,
    const void* data,
    ID3D12Resource* buffer,
    GpuResource::ResourceState state,
    Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer)
{
    auto bufferDescription = buffer->GetDesc();


    // Prepare a pBitmap in memory, with bitmapWidth, bitmapHeight, and pixel format of DXGI_FORMAT_B8G8R8A8_UNORM. 
    //
    // Sub-allocate from the buffer for texture data.
    //

    D3D12_SUBRESOURCE_FOOTPRINT pitchedDesc;
    ZEROS(pitchedDesc);
    pitchedDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    pitchedDesc.Width = bufferDescription.Width;
    pitchedDesc.Height = bufferDescription.Height;
    pitchedDesc.Depth = bufferDescription.DepthOrArraySize;
    pitchedDesc.RowPitch = Align(bufferDescription.Width * BitsPerPixel(bufferDescription.Format) / 8,
        D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);

    //
    // Note that the helper function UpdateSubresource in D3DX12.h, and ID3D12Device::GetCopyableFootprints 
    // can help applications fill out D3D12_SUBRESOURCE_FOOTPRINT and D3D12_PLACED_SUBRESOURCE_FOOTPRINT structures.
    //
    // Refer to the D3D12 Code example for the previous section "Uploading Different Types of Resources"
    // for the code for SuballocateFromBuffer.
    //
    UINT8* m_pDataBegin;
    UINT8* m_pDataCur;
    UINT8* m_pDataEnd;
    auto uSize = bufferDescription.Width *
        bufferDescription.Height *
        bufferDescription.DepthOrArraySize * BitsPerPixel(bufferDescription.Format) / 8;
    ThrowIfFailed(device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES( D3D12_HEAP_TYPE_UPLOAD ),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer( uSize ),
        D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
        IID_PPV_ARGS( &uploadBuffer ) ));

    void* pData;
    //
    // No CPU reads will be done from the resource.
    //
    CD3DX12_RANGE readRange(0, 0);
    uploadBuffer->Map(0, &readRange, &pData);
    m_pDataCur = m_pDataBegin = reinterpret_cast<UINT8*>(pData);
    m_pDataEnd = m_pDataBegin + uSize;

    m_pDataCur = reinterpret_cast<UINT8*>(
        Align(reinterpret_cast<SIZE_T>(m_pDataCur), D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT)
    );

    D3D12_PLACED_SUBRESOURCE_FOOTPRINT placedTexture2D = {0};
    placedTexture2D.Offset = m_pDataCur - m_pDataBegin;
    placedTexture2D.Footprint = pitchedDesc;

    //
    // Copy texture data from DWORD* pBitmap->pixels to the buffer
    //

    for (UINT y = 0; y < bufferDescription.Height; y++)
    {
        UINT8* pScan = m_pDataBegin + placedTexture2D.Offset + y * pitchedDesc.RowPitch;
        memcpy(pScan, &(((uint8_t*)data)[y * bufferDescription.Width]),
            BitsPerPixel(bufferDescription.Format) / 8 * bufferDescription.Width);
    }

    uploadBuffer->Unmap(0,NULL);

    //
    // Copy heap data to texture2D.
    //
    auto transtion = CD3DX12_RESOURCE_BARRIER::Transition(buffer,
        ToDx12(state), D3D12_RESOURCE_STATE_COPY_DEST);

    cmdList->CopyTextureRegion(
        &CD3DX12_TEXTURE_COPY_LOCATION(buffer, 0),
        0, 0, 0,
        &CD3DX12_TEXTURE_COPY_LOCATION(uploadBuffer.Get(), placedTexture2D),
        nullptr);
    transtion = CD3DX12_RESOURCE_BARRIER::Transition(buffer,
        D3D12_RESOURCE_STATE_COPY_DEST, ToDx12(state));
    cmdList->ResourceBarrier(1, &transtion);
}

void UploadBufferData(
    ID3D12Device* device,
    ID3D12GraphicsCommandList* cmdList,
    const void* data,
    const GpuResource& resource,
    Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer)
{
    auto buffer = (ID3D12Resource*)resource.resource;
    auto bufferDescription = buffer->GetDesc();
    //if (bufferDescription.Dimension != D3D12_RESOURCE_DIMENSION_BUFFER)
    //    return UploadTextureData(device, cmdList, data, buffer, state, uploadBuffer);

    auto heapDesUpload = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

    auto resDesc = buffer->GetDesc();;
    UINT64 requiredSize;
    //  = resDesc.Width *
    //    resDesc.Height *
    //    resDesc.DepthOrArraySize * BitsPerPixel(bufferDescription.Format) / 8;
    // In order to copy CPU memory data into our default buffer, we need to create
    // an intermediate upload heap. 
    device->GetCopyableFootprints(&resDesc, 0, 1, 0, nullptr, nullptr, nullptr, &requiredSize);
    ThrowIfFailed(device->CreateCommittedResource(
        &heapDesUpload,
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer( requiredSize ),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&uploadBuffer)));

    // Describe the data we want to copy into the default buffer.
    D3D12_SUBRESOURCE_DATA subResourceData;
    subResourceData.pData = data;

    subResourceData.RowPitch = resource.resourceDescription.Width * BitsPerPixel(resDesc.Format) / 8;
    subResourceData.SlicePitch = subResourceData.RowPitch * resDesc.Height;

    // Schedule to copy the data to the default buffer resource.  At a high level, the helper function UpdateSubresources
    // will copy the CPU memory into the intermediate upload heap.  Then, using ID3D12CommandList::CopySubresourceRegion,
    // the intermediate upload heap data will be copied to mBuffer.
    auto transtion = CD3DX12_RESOURCE_BARRIER::Transition(buffer,
        ToDx12(resource.realState),
        D3D12_RESOURCE_STATE_COPY_DEST);
    cmdList->ResourceBarrier(1, &transtion);

    UpdateSubresources<1>(cmdList, buffer, uploadBuffer.Get(), 0, 0, 1, &subResourceData);
    transtion = CD3DX12_RESOURCE_BARRIER::Transition(buffer,
        D3D12_RESOURCE_STATE_COPY_DEST, ToDx12(resource.realState));
    cmdList->ResourceBarrier(1, &transtion);

    // Note: uploadBuffer has to be kept alive after the above function calls because
    // the command list has not been executed yet that performs the actual copy.
    // The caller can Release the uploadBuffer after it knows the copy has been executed.
}


void RenderDeviceDX12::UploadSubresourceData(const GpuResource& resource, uint16_t dstSubresource,
    const size_t dataSize, const void* pSrcData, int32_t srcRowPitch,
    int32_t srcDepthPitch)
{
    //uploadBuffers.push_back(nullptr);
    UBox box;
    box.Top = 0;
    box.Left = 0;
    box.Front = 0;
    box.Right = dataSize;
    box.Bottom = 1;
    box.Back = 1;
    SetSubresourceData(
        resource,
        dstSubresource,
        box,
        pSrcData,
        srcRowPitch,
        srcDepthPitch
    );
}

void RenderDeviceDX12::MakeUploadBufferBigger(size_t minSize)
{
    currentBufferUploadBufferSize = currentBufferUploadBufferSize * 1.3 + minSize;
    currentBufferUploadBuffer->Unmap(0, &D3D12_RANGE(0, currentBufferUploadBufferShift));

    ThrowIfFailed(md3dDevice->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer( currentBufferUploadBufferSize ),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&currentBufferUploadBuffer)));
    bufferUploadBuffers.push_back(currentBufferUploadBuffer);

    ThrowIfFailed(currentBufferUploadBuffer->Map(0, NULL, reinterpret_cast<void**>(&currentBufferUploadBufferPtr)));
    currentBufferUploadBufferShift = 0;
}

void RenderDeviceDX12::SetSubresourceData(const GpuResource& resource, uint16_t dstSubresource, const UBox& rect,
    const void* pSrcData, int32_t srcRowPitch,
    int32_t srcDepthPitch)
{
    //uploadBuffers.push_back(nullptr);

    auto buffer = (ID3D12Resource*)resource.resource;
    auto bufferDescription = buffer->GetDesc();

    // if (bufferDescription.Dimension != D3D12_RESOURCE_DIMENSION_BUFFER)
    //    uploadBuffers.push_back(nullptr);
    //auto& uploadBuffer = uploadBuffers[uploadBuffers.size() - 1];
    //if (bufferDescription.Dimension != D3D12_RESOURCE_DIMENSION_BUFFER)
    //    return UploadTextureData(device, cmdList, data, buffer, state, uploadBuffer);


    auto transtion = CD3DX12_RESOURCE_BARRIER::Transition(buffer,
        ToDx12(resource.realState),
        D3D12_RESOURCE_STATE_COPY_DEST);
    if (bufferDescription.Dimension != D3D12_RESOURCE_DIMENSION_BUFFER)
    {
        uploadTextureBuffers.push_back(nullptr);
        auto& m_texture = uploadTextureBuffers[uploadTextureBuffers.size() - 1];
        D3D12_RESOURCE_DESC textureDesc = bufferDescription;
        textureDesc.Width = (rect.Right - rect.Left);
        textureDesc.Height = (rect.Bottom - rect.Top);
        textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

        ThrowIfFailed(md3dDevice->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &textureDesc,
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            IID_PPV_ARGS(&m_texture)));

        const UINT64 uploadBufferSize = GetRequiredIntermediateSize(m_texture.Get(), 0, 1);

        // Create the GPU upload buffer.
        //ThrowIfFailed(md3dDevice->CreateCommittedResource(
        //    &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        //    D3D12_HEAP_FLAG_NONE,
        //    &CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize),
        //    D3D12_RESOURCE_STATE_GENERIC_READ,
        //    nullptr,
        //    IID_PPV_ARGS(&uploadBuffer)));

        currentBufferUploadBufferShift = CalcTextureBufferByteOffset(currentBufferUploadBufferShift);
        if (currentBufferUploadBufferShift + uploadBufferSize > currentBufferUploadBufferSize)
            MakeUploadBufferBigger(uploadBufferSize);

        D3D12_SUBRESOURCE_DATA textureData = {};
        textureData.pData = pSrcData;
        textureData.RowPitch = textureDesc.Width * BitsPerPixel(textureDesc.Format) / 8;
        textureData.SlicePitch = textureData.RowPitch * textureDesc.Height;

        UpdateSubresources(mCommandList.Get(), m_texture.Get(), currentBufferUploadBuffer.Get(),
            currentBufferUploadBufferShift, 0, 1, &textureData);
        currentBufferUploadBufferShift += uploadBufferSize;
        D3D12_RESOURCE_BARRIER transtions[2];
        transtions[0] = CD3DX12_RESOURCE_BARRIER::Transition(m_texture.Get(),
            D3D12_RESOURCE_STATE_COPY_DEST,
            D3D12_RESOURCE_STATE_COPY_SOURCE);
        transtions[1] = CD3DX12_RESOURCE_BARRIER::Transition(buffer,
            ToDx12(resource.realState),
            D3D12_RESOURCE_STATE_COPY_DEST);


        mCommandList->ResourceBarrier(2, transtions);

        CD3DX12_TEXTURE_COPY_LOCATION Dst(buffer, dstSubresource);
        CD3DX12_TEXTURE_COPY_LOCATION Src(m_texture.Get(), 0);
        D3D12_BOX box;
        box.back = 0;
        box.left = 0;
        box.top = 0;
        box.right = textureDesc.Width;
        box.bottom = textureDesc.Height;
        box.front = textureDesc.DepthOrArraySize;

        mCommandList->CopyTextureRegion(&Dst, rect.Left, rect.Top, rect.Front, &Src, nullptr);
        transtion = CD3DX12_RESOURCE_BARRIER::Transition(buffer,
            D3D12_RESOURCE_STATE_COPY_DEST, ToDx12(resource.realState));
        mCommandList->ResourceBarrier(1, &transtion);
    }
    else
    {
        //auto heapDesUpload = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

        auto resDesc = buffer->GetDesc();
        auto requiredSize = rect.GetSize() * BitsPerPixel(bufferDescription.Format) / 8;
        // In order to copy CPU memory data into our default buffer, we need to create
        // an intermediate upload heap. 
        //md3dDevice->GetCopyableFootprints(&resDesc, 0, 1, 0, nullptr, nullptr, nullptr, &requiredSize);
        //ThrowIfFailed(md3dDevice->CreateCommittedResource(
        //    &heapDesUpload,
        //    D3D12_HEAP_FLAG_NONE,
        //    &CD3DX12_RESOURCE_DESC::Buffer( requiredSize ),
        //    D3D12_RESOURCE_STATE_GENERIC_READ,
        //    nullptr,
        //    IID_PPV_ARGS(&uploadBuffer)));

        // Describe the data we want to copy into the default buffer.
        D3D12_SUBRESOURCE_DATA subResourceData;
        subResourceData.pData = pSrcData;

        subResourceData.RowPitch = (rect.Right - rect.Left) * BitsPerPixel(resDesc.Format) / 8;
        subResourceData.SlicePitch = subResourceData.RowPitch * (rect.Bottom - rect.Top);

        // Schedule to copy the data to the default buffer resource.  At a high level, the helper function UpdateSubresources
        // will copy the CPU memory into the intermediate upload heap.  Then, using ID3D12CommandList::CopySubresourceRegion,
        // the intermediate upload heap data will be copied to mBuffer.

        auto pDestinationResource = buffer;

        // BYTE* pData;
        //HRESULT hr = uploadBuffer->Map(0, NULL, reinterpret_cast<void**>(&pData));
        //if (FAILED(hr))
        //{
        //    return;
        //}
        if (currentBufferUploadBufferShift + requiredSize > currentBufferUploadBufferSize)
            MakeUploadBufferBigger(requiredSize);

        D3D12_MEMCPY_DEST DestData = {
            currentBufferUploadBufferPtr + currentBufferUploadBufferShift,requiredSize,requiredSize
        };
        MemcpySubresource(&DestData, &subResourceData, requiredSize, 1, 1);

        //uploadBuffer->Unmap(0, NULL);

        transtion = CD3DX12_RESOURCE_BARRIER::Transition(buffer,
            ToDx12(resource.realState), D3D12_RESOURCE_STATE_COPY_DEST);
        mCommandList->ResourceBarrier(1, &transtion);
        mCommandList->CopyBufferRegion(buffer, rect.Left, currentBufferUploadBuffer.Get(),
            currentBufferUploadBufferShift, requiredSize);
        currentBufferUploadBufferShift += requiredSize;
        transtion = CD3DX12_RESOURCE_BARRIER::Transition(buffer,
            D3D12_RESOURCE_STATE_COPY_DEST, ToDx12(resource.realState));
        mCommandList->ResourceBarrier(1, &transtion);
    }

    //todo Обсудить ибо грустно и 0 идей... Вроде идея есть.... текстуры говно....
}

void RenderDeviceDX12::SyncResourcesState(std::vector<ResourceStateTransition>& transitions)
{
    if (transitions.size() == 0)
        return;
    static std::vector<D3D12_RESOURCE_BARRIER> bariers;
    bariers.clear();
    for (int i = 0; i < transitions.size(); i++)
    {
        if (transitions[i].flags != ResourceStateTransition::DefInvalidFlag &&
            transitions[i].StateTo != transitions[i].StateFrom &&
            Resources.size() > (uintptr_t)transitions[i].resource &&
            Resources[(uintptr_t)transitions[i].resource] != nullptr
        )
        {
            bariers.push_back(
                CD3DX12_RESOURCE_BARRIER::Transition(
                    Resources[(uintptr_t)transitions[i].resource],
                    ToDx12(transitions[i].StateFrom),
                    ToDx12(transitions[i].StateTo),
                    D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
                    transitions[i].flags & ResourceStateTransition::BEGIN ? D3D12_RESOURCE_BARRIER_FLAG_BEGIN_ONLY : D3D12_RESOURCE_BARRIER_FLAG_END_ONLY)
            );
        }
    }
    if (bariers.size() > 0)
        mCommandList->ResourceBarrier(bariers.size(), bariers.data());
}


void RenderDeviceDX12::SetupPipeline(const PipelineDescription& Pipeline)
{
    if (Pipeline.isCS)
    {
        D3D12_COMPUTE_PIPELINE_STATE_DESC ps{};
        ZEROS(ps);
        SignatureParams params;
        params.uaCount = Pipeline.uaCount;
        params.cbCount = Pipeline.cbCount;
        params.srvCount = Pipeline.srvCount;
        params.samplersCount = Pipeline.samplersNum;
        auto rs = FetchRS(params);
        mCommandList->SetComputeRootSignature(rs);
        auto handlesSRV = mGpuShvCbUaHeapInterface->GetCurrentGpuWriteHandle();

        ps.CS = shaders[((uint32_t)Pipeline.CS) - 1];
        ps.pRootSignature = rs;

        wrl::ComPtr<ID3D12PipelineState> newPipeline;
        string_id psid = SIDRT((char*)&ps, sizeof(ps));
        if (pipelineStates.count(psid) > 0)
        {
            newPipeline = pipelineStates[psid];
        }
        else
        {
            std::wstring psName = L"Pipeline_";
            psName.append(std::to_wstring(psid));
            ThrowIfFailed(md3dDevice->CreateComputePipelineState(&ps, IID_PPV_ARGS(&newPipeline)));
            pipelineStates.insert({psid,newPipeline});
            newPipeline->SetName(psName.c_str());
        }
        if (currentPipeline != newPipeline)
        {
            currentPipeline = newPipeline;
            mCommandList->SetPipelineState(newPipeline.Get());
        }


        ID3D12DescriptorHeap* ppHeaps[] = {
            mGpuShvCbUaHeapInterface->Heap(),
            mGpuSamplerHeapInterface->Heap()
        };
        if (ppCuurentHeaps[0] != ppHeaps[0] || ppCuurentHeaps[1] != ppHeaps[0])
        {
            mCommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
        }
        mCommandList->SetComputeRootDescriptorTable(0, handlesSRV);
        SetupSamplers(Pipeline.samplers, Pipeline.samplersNum, true);
    }
    else
    {
        wrl::ComPtr<ID3D12PipelineState> newPipeline;

        D3D12_GRAPHICS_PIPELINE_STATE_DESC ps{};
        ZEROS(ps);
        SignatureParams params;
        params.uaCount = Pipeline.uaCount;
        params.cbCount = Pipeline.cbCount;
        params.srvCount = Pipeline.srvCount;
        params.samplersCount = Pipeline.samplersNum;
        auto rs = FetchRS(params);
        ps.pRootSignature = rs;
        auto handlesSRV = mGpuShvCbUaHeapInterface->GetCurrentGpuWriteHandle();

        if (Pipeline.HS)
            ps.HS = shaders[((uint32_t)Pipeline.HS) - 1];
        if (Pipeline.PS)
            ps.PS = shaders[((uint32_t)Pipeline.PS) - 1];
        if (Pipeline.DS)
            ps.DS = shaders[((uint32_t)Pipeline.DS) - 1];
        if (Pipeline.VS)
            ps.VS = shaders[((uint32_t)Pipeline.VS) - 1];
        if (Pipeline.GS)
            ps.GS = shaders[((uint32_t)Pipeline.GS) - 1];
        ps.InputLayout = inputLayouts[(uint32_t)Pipeline.layout - 1];

        //SetupInputLayout(Pipeline.layout);
        //
        SetupViewports(Pipeline.viewports, Pipeline.viewportsNum);
        //SetupBlendState(Pipeline.blendState);
        //SetupDepthStencilState(Pipeline.depthStencilState);
        //SetupRasterizerState(Pipeline.rasterizerState);
        //SetupPrimitiveTopology(Pipeline.topology);
        ps.BlendState = ToD3D12Blend(Pipeline.blendState);
        ps.DSVFormat = ToD3D_DepthFormat[to_underlying(Pipeline.DSVFormat)];
        ps.RasterizerState = ToDX12RSState(Pipeline.rasterizerState);
        ps.DepthStencilState = ToDX12DSState(Pipeline.depthStencilState);
        ps.PrimitiveTopologyType = ToD3DPT_Type[to_underlying(Pipeline.topology)];
        ps.NumRenderTargets = Pipeline.rtCount;
        ps.SampleDesc.Count = 1;
        ps.SampleDesc.Quality = 0;
        ps.SampleMask = UINT_MAX;
        for (int i = 0; i < Pipeline.rtCount; i++)
        {
            ps.RTVFormats[i] = ToD3D_TextureFormat[to_underlying(Pipeline.RTVFormats[i])];
        }
        ps.SampleDesc.Quality = 0;
        string_id psid = SIDRT((char*)&ps, sizeof(ps));

        if (pipelineStates.count(psid) > 0)
        {
            newPipeline = pipelineStates[psid];
        }
        else
        {
            std::wstring psName = L"Pipeline_";
            psName.append(std::to_wstring(psid));
            ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&ps, IID_PPV_ARGS(&newPipeline)));
            pipelineStates.insert({psid,newPipeline});
            newPipeline->SetName(psName.c_str());
        }

        if (currentPipeline != newPipeline)
        {
            currentPipeline = newPipeline;
            mCommandList->SetGraphicsRootSignature(rs);
            currentRoot = rs;
            mCommandList->SetPipelineState(newPipeline.Get());
            currentTopology = ToD3DPT[to_underlying(Pipeline.topology)];
            mCommandList->IASetPrimitiveTopology(ToD3DPT[to_underlying(Pipeline.topology)]);
        }
        
        if (currentRoot != rs)
        {
            currentRoot = rs;
            mCommandList->SetPipelineState(newPipeline.Get());
        }

        if (
            CurrentBlendFactor[0] != Pipeline.blendState.desc.BlendFactor[0] ||
            CurrentBlendFactor[1] != Pipeline.blendState.desc.BlendFactor[1] ||
            CurrentBlendFactor[2] != Pipeline.blendState.desc.BlendFactor[2] ||
            CurrentBlendFactor[3] != Pipeline.blendState.desc.BlendFactor[3]
        )
        {
            CurrentBlendFactor[0] = Pipeline.blendState.desc.BlendFactor[0];
            CurrentBlendFactor[1] = Pipeline.blendState.desc.BlendFactor[1];
            CurrentBlendFactor[2] = Pipeline.blendState.desc.BlendFactor[2];
            CurrentBlendFactor[3] = Pipeline.blendState.desc.BlendFactor[3];
            mCommandList->OMSetBlendFactor(Pipeline.blendState.desc.BlendFactor);
        }

        if (currentTopology != ToD3DPT[to_underlying(Pipeline.topology)])
        {
            currentTopology = ToD3DPT[to_underlying(Pipeline.topology)];
            mCommandList->IASetPrimitiveTopology(ToD3DPT[to_underlying(Pipeline.topology)]);
        }


        ID3D12DescriptorHeap* ppHeaps[] = {
            mGpuShvCbUaHeapInterface->Heap(),
            mGpuSamplerHeapInterface->Heap()
        };
        if (ppCuurentHeaps[0] != ppHeaps[0] || ppCuurentHeaps[1] != ppHeaps[0])
        {
            mCommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
        }
        if (currentDH.ptr != handlesSRV.ptr)
        {
            currentDH = handlesSRV;
            mCommandList->SetGraphicsRootDescriptorTable(0, handlesSRV);
        }
        SetupSamplers(Pipeline.samplers, Pipeline.samplersNum, false);
    }
}

D3D12_VIEWPORT d3d12viewports[20];
D3D12_RECT m_scissorRect[20];

void RenderDeviceDX12::SetupViewports(const Compressed::ViewportDesc viewports[], uint8_t num)
{
    if (ToD3D12Viewports(viewports, d3d12viewports, m_scissorRect, num, d3d11viewportsNum))
    {
        d3d11viewportsNum = num;
        mCommandList->RSSetViewports(num, d3d12viewports);
        mCommandList->RSSetScissorRects(num, m_scissorRect);
    }
}


D3D12_CPU_DESCRIPTOR_HANDLE RenderDeviceDX12::FetchSamplerState(const Compressed::SamplerStateDesc& state)
{
    if (hashSS.contains(state.Data))
        return hashSS[state.Data];

    D3D12_SAMPLER_DESC desc{};
    assert(state.Fields.Filter != 0);
    desc.Filter = ToD3D11SamplerFilter[state.Fields.Filter];
    desc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    desc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    desc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    desc.MaxAnisotropy = state.Fields.MaxAnisotropy;
    desc.MaxLOD = state.MaxLOD;
    desc.MinLOD = state.MinLOD;
    desc.MipLODBias = 0;
    auto handle = samplersStorageHeap->GetNextCpuHandle();
    md3dDevice->CreateSampler(
        &desc,
        handle
    );
    hashSS.insert({state.Data,handle});
    return handle;
}

void RenderDeviceDX12::SetupSamplers(const Compressed::SamplerStateDesc samplers[], uint8_t num, bool isCompute)
{
    bool flag = cachedSamplersNUm == num;
    for (int i = 0; (i < num) && (flag); i++)
    {
        flag = flag && (cachedSamplers[i] == samplers[i]);
    }
    if (!flag)
    {
        auto handlesSamplers = mGpuSamplerHeapInterface->GetCurrentGpuWriteHandle();
        cachedSamplerhandle = handlesSamplers;
        for (int i = 0; i < num; i++)
        {
            cachedSamplers[i] = samplers[i];
            auto handle = FetchSamplerState((samplers[i]));
            mGpuSamplerHeapInterface->WriteNextDescriptor(md3dDevice.Get(), handle);
        }
        cachedSamplersNUm = num;
        if (isCompute)
            mCommandList->SetComputeRootDescriptorTable(1, handlesSamplers);
        else
            mCommandList->SetGraphicsRootDescriptorTable(1, handlesSamplers);
    }
    else
    {
        //if (isCompute)
        //    mCommandList->SetComputeRootDescriptorTable(1, cachedSamplerhandle);
        //else
        //    mCommandList->SetGraphicsRootDescriptorTable(1, cachedSamplerhandle);
    }
}

void RenderDeviceDX12::ClearRenderTarget(RENDERTARGETVIEWHANDLE rtView, FColor color)
{
    D3D12_CPU_DESCRIPTOR_HANDLE handle;
    handle.ptr = rtView.data;
    mCommandList->ClearRenderTargetView(handle, color.Color, 0, NULL);
}

void RenderDeviceDX12::ClearDepthStencil(DEPTHSTENCILVIEWHANDLE dsView, float depth, int8_t stencil)
{
    D3D12_CPU_DESCRIPTOR_HANDLE handle;
    handle.ptr = dsView.data;
    mCommandList->ClearDepthStencilView(handle, D3D12_CLEAR_FLAGS(0x1 | 0x2), depth, stencil, 0, NULL);
}

void RenderDeviceDX12::FlushCommandQueue()
{
    // Advance the fence value to mark commands up to this fence point.
    mCurrentFence++;

    // Add an instruction to the command queue to set a new fence point.  Because we 
    // are on the GPU timeline, the new fence point won't be set until the GPU finishes
    // processing all the commands prior to this Signal().
    ThrowIfFailed(mCommandQueue->Signal(mFence.Get(), mCurrentFence));

    // Wait until the GPU has completed commands up to this fence point.
    if (mFence->GetCompletedValue() < mCurrentFence)
    {
        HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);

        // Fire event when GPU hits current fence.  
        ThrowIfFailed(mFence->SetEventOnCompletion(mCurrentFence, eventHandle));

        // Wait until the GPU hits current fence event is fired.
        WaitForSingleObject(eventHandle, INFINITE);
        CloseHandle(eventHandle);
    }
}

void RenderDeviceDX12::Present()
{
    // Done recording commands.

    mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        D3D12_RESOURCE_STATE_PRESENT));

    ThrowIfFailed(mCommandList->Close());

    currentBufferUploadBuffer->Unmap(0, &D3D12_RANGE(0, currentBufferUploadBufferShift));

    // Add the command list to the queue for execution.
    ID3D12CommandList* cmdsLists[] = {mCommandList.Get()};

    mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);


    // swap the back and front buffers
    ThrowIfFailed(mSwapChain->Present(0, 0));

    // Wait until frame commands are complete.  This waiting is inefficient and is
    // done for simplicity.  Later we will show how to organize our rendering code
    // so we do not have to wait per frame.
    mCurrentFence++;

    // Add an instruction to the command queue to set a new fence point.  Because we 
    // are on the GPU timeline, the new fence point won't be set until the GPU finishes
    // processing all the commands prior to this Signal().
    ThrowIfFailed(mCommandQueue->Signal(mFence.Get(), mCurrentFence));

    Frames[mCurrBackBuffer].mDirectCmdListAlloc = mDirectCmdListAlloc;
    Frames[mCurrBackBuffer].mCommandList = mCommandList;
    Frames[mCurrBackBuffer].uploadBuffers = std::move(bufferUploadBuffers);
    Frames[mCurrBackBuffer].uploadTextureBuffers = std::move(uploadTextureBuffers);
    Frames[mCurrBackBuffer].uploadBuffer = currentBufferUploadBuffer;
    Frames[mCurrBackBuffer].fenceValue = mCurrentFence;
    Frames[mCurrBackBuffer].mGpuSamplerHeapInterface = mGpuSamplerHeapInterface;
    Frames[mCurrBackBuffer].mGpuShvCbUaHeapInterface = mGpuShvCbUaHeapInterface;
    Frames[mCurrBackBuffer].currentBufferUploadBufferSize = currentBufferUploadBufferSize;
    
///thread Shinanigans
///
    mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;
    
    // Wait until the GPU has completed commands up to this fence point.
    if (mFence->GetCompletedValue() < Frames[mCurrBackBuffer].fenceValue)
    {
        HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);

        // Fire event when GPU hits current fence.  
        ThrowIfFailed(mFence->SetEventOnCompletion(Frames[mCurrBackBuffer].fenceValue, eventHandle));

        // Wait until the GPU hits current fence event is fired.
        WaitForSingleObject(eventHandle, INFINITE);
        CloseHandle(eventHandle);
    }
    
    Frames[mCurrBackBuffer].uploadBuffers.clear();
    Frames[mCurrBackBuffer].uploadTextureBuffers.clear();
    currentBufferUploadBuffer =  Frames[mCurrBackBuffer].uploadBuffer;
    mCommandList =  Frames[mCurrBackBuffer].mCommandList;
    mDirectCmdListAlloc =  Frames[mCurrBackBuffer].mDirectCmdListAlloc;
    mGpuSamplerHeapInterface =  Frames[mCurrBackBuffer].mGpuSamplerHeapInterface;
    mGpuShvCbUaHeapInterface =  Frames[mCurrBackBuffer].mGpuShvCbUaHeapInterface;
    currentBufferUploadBufferSize =  Frames[mCurrBackBuffer].currentBufferUploadBufferSize;
    
    bufferUploadBuffers.clear();
    bufferUploadBuffers.push_back(currentBufferUploadBuffer);
    
    mGpuShvCbUaHeapInterface->next = 0;
    mGpuSamplerHeapInterface->next = 0;
    mGpuShvCbUaHeapInterface->writeNext = 0;
    mGpuSamplerHeapInterface->writeNext = 0;


    currentBufferUploadBufferShift = 0;
    currentBufferUploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&currentBufferUploadBufferPtr));

    // Reuse the memory associated with command recording.
    // We can only reset when the associated command lists have finished execution on the GPU.
    ThrowIfFailed(mDirectCmdListAlloc->Reset());

    // A command list can be reset after it has been added to the command queue via ExecuteCommandList.
    // Reusing the command list reuses memory.
    ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));


    mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
        D3D12_RESOURCE_STATE_PRESENT,
        D3D12_RESOURCE_STATE_RENDER_TARGET));

    auto color = {0.f,0.f,0.f,1.f};
    mCommandList->ClearRenderTargetView(CurrentBackBufferView(), color.begin(), 0, NULL);

}

void RenderDeviceDX12::SetupVertexBuffers(const VERTEXBUFFERVIEWHANDLE vertexBuffersOut[], uint8_t num)
{
    bool setup = false;
    for (int i = 0; i < num; i++)
    {
        const auto vb = (vertexBuffersOut[i].data);

        if (vb != 0)
        {
            if (vertexBuffers[vb - 1].BufferLocation != this->vertexBuffersPtr[i].BufferLocation)
            {
                this->vertexBuffersPtr[i] = vertexBuffers[vb - 1];
                setup = true;
            }
        }
        else
        {
            setup = true;
            this->vertexBuffersPtr[i] = {};
        }
    }
    if (setup)
        mCommandList->IASetVertexBuffers(0, num, this->vertexBuffersPtr.data());
}

void RenderDeviceDX12::SetupIndexBuffer(const INDEXBUFFERVIEWHANDLE indices)
{
    const auto ib = (indices.data);
    if (ib != 0)
    {
        if (currentIndexBuffer != ib)
        {
            currentIndexBuffer = ib;
            mCommandList->IASetIndexBuffer(&indexBuffers[ib - 1]);
        }
    }
    else
    {
        currentIndexBuffer = 0;
        mCommandList->IASetIndexBuffer(nullptr);
    }
}

void RenderDeviceDX12::SetupTextures(RESOURCEVIEWHANDLE textures[], uint8_t num)
{
    static D3D12_CPU_DESCRIPTOR_HANDLE handle;
    for (int i = 0; i < num; i++)
    {
        if (textures != nullptr && i < num && textures[i].data != 0)
            handle.ptr = textures[i].data;
        else
            handle.ptr = defaultTexture.ptr;
        mGpuShvCbUaHeapInterface->WriteNextDescriptor(md3dDevice.Get(), handle);
    }
}

void RenderDeviceDX12::SetupUATargets(UATARGETVIEWHANDLE ua_targets[], uint8_t num)
{
    static D3D12_CPU_DESCRIPTOR_HANDLE handle;
    for (int i = 0; i < num; i++)
    {
        if (ua_targets != nullptr && ua_targets[i].data != 0)
            handle.ptr = ua_targets[i].data;
        mGpuShvCbUaHeapInterface->WriteNextDescriptor(md3dDevice.Get(), handle);
    }
}

void RenderDeviceDX12::SetupConstBuffers(CONSTBUFFERVIEWHANDLE constBuffers[], uint8_t num)
{
    static D3D12_CPU_DESCRIPTOR_HANDLE handle;
    for (int i = 0; i < num; i++)
    {
        if (constBuffers != nullptr && constBuffers[i].data != 0)
            handle.ptr = constBuffers[i].data;
        else
            handle.ptr = defaultCB.ptr;
        mGpuShvCbUaHeapInterface->WriteNextDescriptor(md3dDevice.Get(), handle);
    }
}

void RenderDeviceDX12::SetupRenderTargets(const RENDERTARGETVIEWHANDLE renderTargets[], int32_t num,
    DEPTHSTENCILVIEWHANDLE depthStencilBuffer)
{
    D3D12_CPU_DESCRIPTOR_HANDLE handle;
    bool setup = false;
    for (int i = 0; i < DX12Limitations::maxRTV; i++)
    {
        if (renderTargets != nullptr && i < num)
        {
            handle.ptr = renderTargets[i].data;
            if (renderTargets[i].data == 0)
                handle = CurrentBackBufferView();
            if (renderTargetsHandles[i].ptr != handle.ptr)
            {
                renderTargetsHandles[i].ptr = handle.ptr;
                setup = true;
            }
        }
    }
    if (depthStencilBuffer.data != 0)
    {
        if (currentDSV.ptr != depthStencilBuffer.data)
        {
            currentDSV.ptr = depthStencilBuffer.data;
            setup = true;
        }

        if (setup)
        {
            mCommandList->OMSetRenderTargets(num, renderTargetsHandles,
                false,
                &currentDSV);
        }
    }
    else
    {
        mCommandList->OMSetRenderTargets(num,
            renderTargetsHandles,
            false,
            nullptr);
    }
}


void RenderDeviceDX12::ClearState()
{
    mCommandList->ClearState(nullptr);
}

void RenderDeviceDX12::Draw(const DrawCall& call)
{
    switch (call.type)
    {
    case EDrawCallType::DRAW_INDEXED:
    case EDrawCallType::DRAW:
    case EDrawCallType::DRAW_INDEXED_INSTANCED:
    {
        mCommandList->DrawIndexedInstanced(
            call.IndexCountPerInstance,
            call.InstanceCount,
            call.StartIndexLocation,
            call.BaseVertexLocation,
            call.StartInstanceLocation);
        break;
    }
    case EDrawCallType::DISPATCH:
    {
        mCommandList->Dispatch(call.x, call.y, call.z);
        break;
    }
    }
}

void RenderDeviceDX12::GetBackbufferSize(uint32_t& w, uint32_t& h)
{
    w = BackBufferWidth;
    h = BackBufferHeight;
}

#include "pix3.h"

void RenderDeviceDX12::BeginEvent(const char* name)
{
    PIXBeginEvent(PIX_COLOR(255, 0, 0), name);
}

void GVM::RenderDeviceDX12::EndEvent()
{
    PIXEndEvent();
}
