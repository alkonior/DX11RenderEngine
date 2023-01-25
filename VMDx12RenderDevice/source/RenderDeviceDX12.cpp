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
		pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);
		// Suppress whole categories of messages
		//D3D12_MESSAGE_CATEGORY Categories[] = {};

		// Suppress messages based on their severity level
		D3D12_MESSAGE_SEVERITY Severities[] =
		{
			D3D12_MESSAGE_SEVERITY_INFO
		};

		// Suppress individual messages by their ID
		D3D12_MESSAGE_ID DenyIds[] = {
			D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,   // I'm really not sure how to avoid this message.
			D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,                         // This warning occurs when using capture frame while graphics debugging.
			D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,                       // This warning occurs when using capture frame while graphics debugging.
		};

		D3D12_INFO_QUEUE_FILTER NewFilter = {};
		//NewFilter.DenyList.NumCategories = _countof(Categories);
		//NewFilter.DenyList.pCategoryList = Categories;
		NewFilter.DenyList.NumSeverities = _countof(Severities);
		NewFilter.DenyList.pSeverityList = Severities;
		NewFilter.DenyList.NumIDs = _countof(DenyIds);
		NewFilter.DenyList.pIDList = DenyIds;

		ThrowIfFailed(pInfoQueue->PushStorageFilter(&NewFilter));
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

	try
	{
		CreateCommandObjects(initParams);
		CreateSwapChain(initParams);
		CreateRtvAndDsvDescriptorHeaps();

		/*

		D3D12_GRAPHICS_PIPELINE_STATE_DESC ps;
		ps.RasterizerState = {
			.FillMode = D3D12_FILL_MODE_SOLID,
			.CullMode = D3D12_CULL_MODE_FRONT,
		};
		ps.HS = {nullptr, 0};
		ps.DS = {nullptr, 0};
		ps.VS = {nullptr, 0};
		ps.PS = {nullptr, 0};
		ps.GS = {nullptr, 0};
		ps.InputLayout = {
			.pInputElementDescs = nullptr,
			.NumElements = 0,
		};
		ps.DepthStencilState = {
				.DepthEnable = false
			},
			ps.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		ps.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		ps.NumRenderTargets = 0;
		ps.SampleDesc.Count = 0;
		ps.BlendState = ToD3D12Blend({});
		ps.pRootSignature = FetchRS({});
		ps.BlendState.IndependentBlendEnable=false;
		ps.BlendState.AlphaToCoverageEnable=false;
		ps.StreamOutput.NumEntries = 0;
		ps.StreamOutput.NumStrides = 0;
		md3dDevice->CreateGraphicsPipelineState(&ps, IID_PPV_ARGS(&mDefaultPS));
		 */


		 //CreateDefaultHandles();
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


	// Reuse the memory associated with command recording.
	// We can only reset when the associated command lists have finished execution on the GPU.
	ThrowIfFailed(mDirectCmdListAlloc->Reset());

	// A command list can be reset after it has been added to the command queue via ExecuteCommandList.
	// Reusing the command list reuses memory.
	ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));
}

void RenderDeviceDX12::CreateSwapChain(const RenderDeviceInitParams& initParams)
{
	// Release the previous swapchain we will be recreating.
	mSwapChain.Reset();

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
	}





	//GPU Descriptor heaps
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

	ID3D12DescriptorHeap* ppHeaps[] = { mGpuShvCbUaHeapInterface->Heap(),mGpuSamplerHeapInterface->Heap() };
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
void RenderDeviceDX12::CreateDefaultHandles()
{
	// Create the actual default buffer resource.
	ThrowIfFailed(md3dDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(256),
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(cbResource.GetAddressOf())));

	D3D12_CONSTANT_BUFFER_VIEW_DESC desc;
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
		&CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM, 256, 256),
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(shResource.GetAddressOf())));


	auto defaultTexture = srvStorageHeap->GetNextCpuHandle();
	md3dDevice->CreateShaderResourceView(
		shResource.Get(),
		nullptr,
		defaultTexture
	);


	ThrowIfFailed(md3dDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM, 256, 256, 8),
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(rtResource.GetAddressOf())));

	for (int i = 0; i < 8; i++)
	{
		defaultRT[i] = srvStorageHeap->GetNextCpuHandle();
		D3D12_RENDER_TARGET_VIEW_DESC desc;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.MipSlice = 0;
		desc.Texture2DArray.PlaneSlice = 0;
		desc.Texture2DArray.ArraySize = 1;
		desc.Texture2DArray.FirstArraySlice = i;

		md3dDevice->CreateRenderTargetView(
			rtResource.Get(),
			&desc,
			defaultRT[i]
		);
	}

	ThrowIfFailed(md3dDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM, 256, 256, 8),
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(uaResource.GetAddressOf())));

	for (int i = 0; i < 8; i++)
	{
		defaultUA[i] = srvStorageHeap->GetNextCpuHandle();
		D3D12_UNORDERED_ACCESS_VIEW_DESC desc;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
		desc.Texture2DArray.MipSlice = 0;
		desc.Texture2DArray.PlaneSlice = 0;
		desc.Texture2DArray.ArraySize = 1;
		desc.Texture2DArray.FirstArraySlice = i;

		md3dDevice->CreateUnorderedAccessView(
			rtResource.Get(),
			nullptr,
			&desc,
			defaultUA[i]
		);
	}

	defaultSampler = FetchSamplerState({});
}

IRenderDevice::IResource* RenderDeviceDX12::CreateResource(const GpuResource& ResourceDesc)
{
	switch (ResourceDesc.resourceDescription.Dimension)
	{
	case EResourceDimension::RESOURCE_DIMENSION_BUFFER:
	{
		ID3D12Resource* defaultBuffer;

		// Create the actual default buffer resource.
		ThrowIfFailed(md3dDevice->CreateCommittedResource(
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
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(&defaultBuffer)));

		return (IRenderDevice::IResource*)defaultBuffer;
	}
	case EResourceDimension::RESOURCE_DIMENSION_TEXTURE2D:
	{
		ID3D12Resource* defaultBuffer;

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
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(&defaultBuffer)));

		return (IRenderDevice::IResource*)defaultBuffer;
	}
	case EResourceDimension::RESOURCE_DIMENSION_TEXTURE3D:
	{
		ID3D12Resource* defaultBuffer;

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
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(&defaultBuffer)));

		return (IRenderDevice::IResource*)defaultBuffer;
	}
	default: break;
	}
	return nullptr;
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
		[params](const std::pair<uint32_t, wrl::ComPtr<ID3D12RootSignature>>& i) { return i.first == params.data; }))
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
			CD3DX12_DESCRIPTOR_RANGE textureSamplerRange(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, DX12Limitations::maxSamplers, 0);
			rootParameters[RootParameterIndex::SamplerBase].InitAsDescriptorTable(
				1, &textureSamplerRange, D3D12_SHADER_VISIBILITY_ALL);
		}
		// HLSL syntax todo!!!! maybe...
		//#define DualTextureRS \
        //"RootFlags ( ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |" \
        //"            DENY_DOMAIN_SHADER_ROOT_ACCESS |" \
        //"            DENY_GEOMETRY_SHADER_ROOT_ACCESS |" \
        //"            DENY_HULL_SHADER_ROOT_ACCESS )," \
        //"DescriptorTable ( SRV(t0, numDescriptors = 2), visibility = SHADER_VISIBILITY_PIXEL )," \
        //"DescriptorTable ( Sampler(s0, numDescriptors = 2), visibility = SHADER_VISIBILITY_PIXEL )," \
        //"CBV(b0)"

		// Create the root signature

		CD3DX12_ROOT_SIGNATURE_DESC rsigDesc = {};
		auto paramsEnum = (descriptorRanges.size() > 0) + (params.samplersCount > 0) * 2;
		switch (paramsEnum)
		{
		case 0://NO params at all
		{
			rsigDesc.Init(0, nullptr, 0, nullptr, rootSignatureFlags);
			break;
		};
		case 1://NO sampler
		{
			rsigDesc.Init(1, rootParameters, 0, nullptr, rootSignatureFlags);
			break;
		};
		case 2://NO cb,ua,shv
		{
			rsigDesc.Init(1, rootParameters + 1, 0, nullptr, rootSignatureFlags);
			break;
		};
		case 3:// samplers+all...
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
		try
		{
			ThrowIfFailed(D3D12SerializeRootSignature(&rsigDesc, D3D_ROOT_SIGNATURE_VERSION_1, pSignature.GetAddressOf(),
				pError.GetAddressOf()));
		}
		catch (std::exception& ex)
		{
			GVM::CompileException ce{
				__LINE__, __FILE__,(hr),infoManager.GetMessages(),(char*)pError->GetBufferPointer(),"Root Signature."
			};
			throw ce;
		}
		ThrowIfFailed(md3dDevice->CreateRootSignature(0, pSignature->GetBufferPointer(), pSignature->GetBufferSize(),
			IID_PPV_ARGS(&rootSignature)));
		RootSignatures.push_back({ params.data,rootSignature });
		rs = rootSignature.Get();
	}

	return rs;
}

void RenderDeviceDX12::DestroyResource(RESOURCEHANDLE const resource)
{
	ID3D12Resource* resourcePtr = (ID3D12Resource*)resource;
	resourcePtr->Release();
}

void RenderDeviceDX12::DestroyResourceView(const RESOURCEVIEWHANDLE resource)
{}

IRenderDevice::SHADERHANDLE RenderDeviceDX12::CreateShader(const ShaderDesc& desc)
{
	shaders.push_back({ desc.bytecode,desc.byteCodeSize });
	return (IRenderDevice::SHADERHANDLE)shaders.size();
}

IRenderDevice::INPUTLAYOUTHANDLE RenderDeviceDX12::CreateInputLayout(const InputAssemblerDeclarationDesc& desc,
	const ShaderDesc& Shader)
{
	inputLayoutsData.push_back(ToD3D11(desc));
	D3D12_INPUT_LAYOUT_DESC inp;
	inp.NumElements = inputLayoutsData.rbegin()->size();
	inp.pInputElementDescs = inputLayoutsData.rbegin()->data();
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
		desc.SizeInBytes = ViewDesc.cbViewDescription.Size;

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

		descriptorFormats.insert({ descriptorHandle.ptr,desc.Format });

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

		descriptorFormats.insert({ descriptorHandle.ptr,desc.Format });

		output.data = descriptorHandle.ptr;
		break;
	}
	case GpuResourceView::EViewType::SR:
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
		const auto& view = ViewDesc.srViewDescription;
		desc = ToD3D12ShaderView(view);

		auto descriptorHandle = srvStorageHeap->GetNextCpuHandle();
		if(!view.MakeDefault)
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
		md3dDevice->CreateUnorderedAccessView(
			resource,
			nullptr,
			&desc,
			descriptorHandle
		);

		descriptorFormats.insert({ descriptorHandle.ptr,desc.Format });
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


void UploadBufferData(
	ID3D12Device* device,
	ID3D12GraphicsCommandList* cmdList,
	const void* data,
	ID3D12Resource* buffer,
	GpuResource::ResourceState state,
	Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer)
{
	auto heapDesUpload = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

	auto resDesc = buffer->GetDesc();;
	// In order to copy CPU memory data into our default buffer, we need to create
	// an intermediate upload heap. 
	ThrowIfFailed(device->CreateCommittedResource(
		&heapDesUpload,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(uploadBuffer.GetAddressOf())));

	// Describe the data we want to copy into the default buffer.
	D3D12_SUBRESOURCE_DATA subResourceData;
	subResourceData.pData = data;
	subResourceData.RowPitch = resDesc.Height * resDesc.Width * BitsPerPixel(resDesc.Format);
	subResourceData.SlicePitch = subResourceData.RowPitch * resDesc.Height;

	// Schedule to copy the data to the default buffer resource.  At a high level, the helper function UpdateSubresources
	// will copy the CPU memory into the intermediate upload heap.  Then, using ID3D12CommandList::CopySubresourceRegion,
	// the intermediate upload heap data will be copied to mBuffer.
	auto transtion = CD3DX12_RESOURCE_BARRIER::Transition(buffer,
		ToDx12(state), D3D12_RESOURCE_STATE_COPY_DEST);
	cmdList->ResourceBarrier(1, &transtion);

	UpdateSubresources<1>(cmdList, buffer, uploadBuffer.Get(), 0, 0, 1, &subResourceData);
	transtion = CD3DX12_RESOURCE_BARRIER::Transition(buffer,
		D3D12_RESOURCE_STATE_COPY_DEST, ToDx12(state));
	cmdList->ResourceBarrier(1, &transtion);

	// Note: uploadBuffer has to be kept alive after the above function calls because
	// the command list has not been executed yet that performs the actual copy.
	// The caller can Release the uploadBuffer after it knows the copy has been executed.
}

void RenderDeviceDX12::SetResourceData(const GpuResource& resourceG, uint16_t dstSubresource, const UBox& rect,
	const void* pSrcData, int32_t srcRowPitch, int32_t srcDepthPitch)
{
	auto resource = (ID3D12Resource*)resourceG.resource;
	auto desc = resource->GetDesc();
	if (desc.Height == rect.Right && rect.Left == 0)
	{
		uploadBuffers.push_back(nullptr);

		UploadBufferData(
			md3dDevice.Get(),
			mCommandList.Get(),
			pSrcData,
			resource,
			resourceG.currentState,
			uploadBuffers[uploadBuffers.size() - 1]
		); //todo Обсудить ибо грустно и 0 идей...
	}
}
void RenderDeviceDX12::SyncResourcesState(GpuResource* data[], size_t size)
{
	if (size == 0)
		return;
	std::vector<D3D12_RESOURCE_BARRIER> bariers;
	for (int i = 0; i < size; i++)
	{
		if (data[i]->currentState != data[i]->nextState)
		{
			bariers.push_back(
				CD3DX12_RESOURCE_BARRIER::Transition(
					(ID3D12Resource*)data[i]->resource,
					ToDx12(data[i]->currentState),
					ToDx12(data[i]->nextState)));
			data[i]->currentState = data[i]->nextState;
		}
	}
	if (bariers.size() > 0)
		mCommandList->ResourceBarrier(bariers.size(), bariers.data());
}


void RenderDeviceDX12::SetupPipeline(const PipelineDescription& Pipeline)
{
	ID3D12PipelineState* newPipeline;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC ps{};
	SignatureParams params;
	params.uaCount = Pipeline.uaCount;
	params.cbCount = Pipeline.cbCount;
	params.srvCount = Pipeline.srvCount;
	ps.pRootSignature = FetchRS(params);
	mCommandList->SetGraphicsRootDescriptorTable(0, mGpuShvCbUaHeapInterface->GetCurrentGpuHandle());
	mCommandList->SetGraphicsRootDescriptorTable(1, mGpuSamplerHeapInterface->GetCurrentGpuHandle());

	if (Pipeline.isCS)
	{
		//    SetupShader(Pipeline.CS, EShaderType::COMPUTE_SHADER);
		//
		//    HS = false;
		//    PS = false;
		//    DS = false;
		//    VS = false;
		//    GS = false;
		//    HSPtr = nullptr;
		//    PSPtr = nullptr;
		//    DSPtr = nullptr;
		//    VSPtr = nullptr;
		//    GSPtr = nullptr;
		//
		//    SetupSamplers(Pipeline.samplers, Pipeline.samplersNum);
		//
		//    
	}
	else
	{
		ps.HS = shaders[((uint32_t)Pipeline.HS)];
		ps.PS = shaders[((uint32_t)Pipeline.PS)];
		ps.DS = shaders[((uint32_t)Pipeline.DS)];
		ps.VS = shaders[((uint32_t)Pipeline.VS)];
		ps.GS = shaders[((uint32_t)Pipeline.GS)];
		ps.InputLayout = inputLayouts[(uint32_t)Pipeline.layout];

		//SetupInputLayout(Pipeline.layout);
		//
		SetupViewports(Pipeline.viewports, Pipeline.viewportsNum);
		SetupSamplers(Pipeline.samplers, Pipeline.samplersNum);
		//SetupBlendState(Pipeline.blendState);
		//SetupDepthStencilState(Pipeline.depthStencilState);
		//SetupRasterizerState(Pipeline.rasterizerState);
		//SetupPrimitiveTopology(Pipeline.topology);
	}
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&ps, IID_PPV_ARGS(&newPipeline)));
	mCommandList->SetPipelineState(newPipeline);
	newPipeline->Release();
}

D3D12_VIEWPORT* d3d12viewports;
void RenderDeviceDX12::SetupViewports(const Compressed::ViewportDesc viewports[], uint8_t num)
{
	if (ToD3D12Viewports(viewports, d3d12viewports, num) || num != d3d11viewportsNum)
	{
		d3d11viewportsNum = num;
		mCommandList->RSSetViewports(num, d3d12viewports);
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
	hashSS.insert({ state.Data,handle });
	return handle;
}

void RenderDeviceDX12::SetupSamplers(const Compressed::SamplerStateDesc samplers[], uint8_t num)
{
	for (int i = 0; i < maxSamplers; i++)
	{
		if (i < num)
		{
			auto handle = FetchSamplerState((samplers[i]));
			mGpuSamplerHeapInterface->WriteNextDescriptor(md3dDevice.Get(), handle);
		}
		//else
		//{
		//    mGpuSamplerHeapInterface->WriteNextDescriptor(md3dDevice.Get(), defaultSampler);
		//}
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
	ThrowIfFailed(mCommandList->Close());

	// Add the command list to the queue for execution.
	ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };

	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
	// swap the back and front buffers
	ThrowIfFailed(mSwapChain->Present(0, 0));
	mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;

	// Wait until frame commands are complete.  This waiting is inefficient and is
	// done for simplicity.  Later we will show how to organize our rendering code
	// so we do not have to wait per frame.
	FlushCommandQueue();


	// Reuse the memory associated with command recording.
	// We can only reset when the associated command lists have finished execution on the GPU.
	ThrowIfFailed(mDirectCmdListAlloc->Reset());

	// A command list can be reset after it has been added to the command queue via ExecuteCommandList.
	// Reusing the command list reuses memory.
	ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));
}

void RenderDeviceDX12::SetupVertexBuffers(const VERTEXBUFFERVIEWHANDLE vertexBuffersOut[], uint8_t num)
{
	vertexBuffersPtr.resize(DX12Limitations::maxVertexBuffers);
	for (int i = 0; i < num; i++)
	{
		const auto vb = (vertexBuffersOut[i].data);
		if (vb != 0)
		{
			this->vertexBuffersPtr[i] = vertexBuffers[vb - 1];
		}
		else
		{
			this->vertexBuffersPtr[i] = {};
		}
	}

	mCommandList->IASetVertexBuffers(0, num, this->vertexBuffersPtr.data());
}

void RenderDeviceDX12::SetupIndexBuffer(const INDEXBUFFERVIEWHANDLE indices)
{
	const auto ib = (indices.data);
	if (ib != 0)
		mCommandList->IASetIndexBuffer(&indexBuffers[ib - 1]);
	else
		mCommandList->IASetIndexBuffer(nullptr);
}

void RenderDeviceDX12::SetupTextures(RESOURCEVIEWHANDLE textures[], uint8_t num)
{
	for (int i = 0; i < DX12Limitations::maxTextures; i++)
	{
		if (textures != nullptr && i < num && textures[i].data != 0)
		{
			D3D12_CPU_DESCRIPTOR_HANDLE handle;
			handle.ptr = textures[i].data;
			mGpuShvCbUaHeapInterface->WriteNextDescriptor(md3dDevice.Get(), handle);
		}
		//else
		//{
		//    mShvHeapInterface->WriteNextDescriptor(md3dDevice.Get(), defaultTexture);
		//}
	}
}

void RenderDeviceDX12::SetupRenderTargets(const RENDERTARGETVIEWHANDLE renderTargets[], int32_t num,
	DEPTHSTENCILVIEWHANDLE depthStencilBuffer)
{
	D3D12_CPU_DESCRIPTOR_HANDLE renderTargetsHandles[8];
	for (int i = 0; i < DX12Limitations::maxRTV; i++)
	{
		if (renderTargets != nullptr && i < num && renderTargets[i].data != 0)
		{
			D3D12_CPU_DESCRIPTOR_HANDLE handle;
			handle.ptr = renderTargets[i].data;
			renderTargetsHandles[i] = mRtvHeapInterface->WriteNextDescriptor(md3dDevice.Get(), handle);
		}
	}
	if (depthStencilBuffer.data != 0)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE handle;
		handle.ptr = depthStencilBuffer.data;
		auto dsv = mDsvHeapInterface->WriteNextDescriptor(md3dDevice.Get(), handle);
		mCommandList->OMSetRenderTargets(num, renderTargetsHandles,
			false,
			&dsv);
	}
	else
	{
		mCommandList->OMSetRenderTargets(num,
			renderTargetsHandles,
			false,
			nullptr);
	}
}

void RenderDeviceDX12::SetupUATargets(UATARGETVIEWHANDLE ua_targets[], uint8_t num)
{
	//D3D12_CPU_DESCRIPTOR_HANDLE renderTargetsHandles[8];
	//for (int i = 0; i < DX12Limitations::maxRTV; i++)
	//{
	//    if (ua_targets != nullptr && i < num)
	//    {
	//        D3D12_CPU_DESCRIPTOR_HANDLE handle;
	//        handle.ptr = ua_targets->data;
	//        mRTVHeapInterface->WriteDescriptor(md3dDevice.Get(), i, handle);
	//    }
	//    else
	//    {
	//        renderTargetsHandles[i] = defaultRT[i];
	//    }
	//}
//
	//    D3D12_CPU_DESCRIPTOR_HANDLE handle;
	//    handle.ptr = depthStencilBuffer.data;
	//    mDsvHeapInterface->WriteDescriptor(md3dDevice.Get(), 0, handle);
	//    mCommandList->Un(DX12Limitations::maxRTV, renderTargetsHandles,
	//                                     false,
	//                                     &handle);
	//
}

void RenderDeviceDX12::SetupConstBuffers(CONSTBUFFERVIEWHANDLE constBuffers[], uint8_t num)
{
	for (int i = 0; i < DX12Limitations::maxConstBuffers; i++)
	{
		if (constBuffers != nullptr && i < num && constBuffers[i].data != 0)
		{
			D3D12_CPU_DESCRIPTOR_HANDLE handle;
			handle.ptr = constBuffers[i].data;
			mGpuShvCbUaHeapInterface->WriteNextDescriptor(md3dDevice.Get(), handle);
		}
		//else
		//{
		//    mCbHeapInterface->WriteNextDescriptor(md3dDevice.Get(), defaultCB);
		//}
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
		mCommandList->DrawIndexedInstanced(call.get<0>(), call.get<1>(), call.get<2>(), call.get<3>(), call.get<4>());
		break;
	}
	case EDrawCallType::DISPATCH:
	{
		mCommandList->Dispatch(call.get<0>(), call.get<1>(), call.get<2>());
		break;
	}
	}
}
void RenderDeviceDX12::GetBackbufferSize(uint32_t& w, uint32_t& h)
{
	w = 1;
	h = 1;
}

void RenderDeviceDX12::BeginEvent(const char* name)
{
	//todo
}
