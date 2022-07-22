#include "RenderDeviceDX11.h"

#include "BuffersViews/ConstBufferViewD3D11.h"
#include "BuffersViews/IndexBufferViewD3D11.h"
#include "BuffersViews/VertexBufferViewD3D11.h"
#include "GraphicsExceptions/GraphicsException.h"
#include "GraphicsVirtualMachine/VirtualMachine/ResourceManager/GPUResource.h"

using namespace GVM;

RenderDeviceDX11::RenderDeviceDX11(const RenderDeviceInitParams& initParams, bool debugMode)
    : IRenderDevice(initParams, debugMode)
{
    //DXGI_ADAPTER_DESC adapterDesc;
    D3D_FEATURE_LEVEL levels[] =
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0
    };
    uint32_t flags;

    /* Load CreateDXGIFactory1 */
    //GFX_THROW_INFO(CreateDXGIFactory(__uuidof(IDXGIFactory), &factory));
    //factory->EnumAdapters(0, adapter.GetAddressOf());
    //
    //adapter->GetDesc(&adapterDesc);

    /* Create the D3D11Device */
    flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
    if (debugMode)
    {
        flags |= D3D11_CREATE_DEVICE_DEBUG;
    }


    DXGI_SWAP_CHAIN_DESC swapchainDesc;

    /* Initialize swapchain descriptor */
    swapchainDesc.BufferDesc.Width = 0;
    swapchainDesc.BufferDesc.Height = 0;
    swapchainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    swapchainDesc.SampleDesc.Count = 1;
    swapchainDesc.SampleDesc.Quality = 0;
    swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapchainDesc.BufferCount = 3;
    swapchainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    //swapchainDesc.BufferDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
    swapchainDesc.Flags = 0;
    swapchainDesc.OutputWindow = (HWND)initParams.deviceWindowHandle.hwnd;

    /* Create the swap chain! */
    //GFX_THROW_INFO(factory->CreateSwapChain(
    //	device.Get(),
    //	&swapchainDesc,
    //	&swapchain
    //));

    GFX_THROW_INFO(D3D11CreateDeviceAndSwapChain(
        NULL, /* FIXME: Should be renderer->adapter! */
        D3D_DRIVER_TYPE_HARDWARE,
        NULL,
        flags,
        NULL,
        0,
        D3D11_SDK_VERSION,
        &swapchainDesc,
        &swapchain,
        &device,
        &featureLevel,
        &context
    ));


    /* Initialize texture and sampler collections */
    Textures.resize(MAX_TEXTURE_ATTACHMENTS, nullptr);

    renderTargetViews.resize(MAX_RENDERTARGET_ATTACHMENTS);

    /* Initialize SetStringMarker support, if available */
    //if (featureLevel == D3D_FEATURE_LEVEL_11_1) {
    //	context->QueryInterface(&annotation);
    //
    //	res = ID3D11DeviceContext_QueryInterface(
    //		renderer->context,
    //		&D3D_IID_ID3DUserDefinedAnnotation,
    //		(void**)&renderer->annotation
    //	);
    //	ERROR_CHECK("Could not get UserDefinedAnnotation")
    //} else {
    //	FNA3D_LogInfo("SetStringMarker not supported!");
    //}

    /* Initialize renderer members not covered by SDL_memset('\0') */
    this->debugMode = debugMode;
    blendFactor = FColor(1, 1, 1, 1);
    multiSampleMask = -1; /* AKA 0xFFFFFFFF, ugh -flibit */
    topology = EPrimitiveTopology::PRIMITIVE_TOPOLOGY_UNKNOWN; /* Force an update */


    context->QueryInterface(__uuidof(ID3DUserDefinedAnnotation), &perf);

    /* Set presentation interval */
    switch (initParams.presentationInterval)
    {
    case EPresentInterval::PRESENT_INTERVAL_ONE:
    case EPresentInterval::PRESENT_INTERVAL_DEFAULT:
    {
        syncInterval = 1;
        break;
    }
    case EPresentInterval::PRESENT_INTERVAL_TWO:
    {
        syncInterval = 2;
        break;
    }
    case EPresentInterval::PRESENT_INTERVAL_IMMEDIATE:
    {
        syncInterval = 0;
        break;
    }
    }


    D3D11_RENDER_TARGET_VIEW_DESC swapchainViewDesc;
    wrl::ComPtr<ID3D11Texture2D> swapchainTexture;


    backBufferWidth = initParams.backBufferWidth;
    backBufferHeight = initParams.backBufferHeight;

    /* Create a render target view for the swapchain */
    swapchainViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapchainViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    swapchainViewDesc.Texture2D.MipSlice = 0;

    GFX_THROW_INFO(swapchain->GetBuffer(
        0,
        __uuidof(ID3D11Texture2D),
        &swapchainTexture
    ));

    GFX_THROW_INFO(device->CreateRenderTargetView(
        swapchainTexture.Get(),
        NULL,
        &swapchainRTView
    ));
}

RenderDeviceDX11::~RenderDeviceDX11()
{}

IStructuresSize RenderDeviceDX11::GetClassesSize()
{
    return {
        sizeof(ResourceDX11),
        sizeof(ResourceViewDX11),
        sizeof(ShaderDX11),
        sizeof(InputLayoutDX11)
    };
}

constexpr DXGI_FORMAT ToD3D_TextureFormat[] =
{
    DXGI_FORMAT_UNKNOWN,

    DXGI_FORMAT_R32G32B32A32_TYPELESS,
    DXGI_FORMAT_R32G32B32A32_FLOAT,
    DXGI_FORMAT_R32G32B32A32_UINT,
    DXGI_FORMAT_R32G32B32A32_SINT,
    DXGI_FORMAT_R16G16B16A16_TYPELESS,
    DXGI_FORMAT_R16G16B16A16_FLOAT,
    DXGI_FORMAT_R16G16B16A16_UNORM,
    DXGI_FORMAT_R16G16B16A16_UINT,
    DXGI_FORMAT_R16G16B16A16_SNORM,
    DXGI_FORMAT_R16G16B16A16_SINT,
    DXGI_FORMAT_R10G10B10A2_TYPELESS,
    DXGI_FORMAT_R10G10B10A2_UNORM,
    DXGI_FORMAT_R10G10B10A2_UINT,
    DXGI_FORMAT_R8G8B8A8_TYPELESS,
    DXGI_FORMAT_R8G8B8A8_UNORM,
    DXGI_FORMAT_B8G8R8A8_UNORM,
    DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
    DXGI_FORMAT_R8G8B8A8_UINT,
    DXGI_FORMAT_R8G8B8A8_SNORM,
    DXGI_FORMAT_R8G8B8A8_SINT,
    //3 chanel
    DXGI_FORMAT_R32G32B32_TYPELESS,
    DXGI_FORMAT_R32G32B32_FLOAT,
    DXGI_FORMAT_R32G32B32_UINT,
    DXGI_FORMAT_R32G32B32_SINT,
    //2 chanel
    DXGI_FORMAT_R32G32_TYPELESS,
    DXGI_FORMAT_R32G32_FLOAT,
    DXGI_FORMAT_R32G32_UINT,
    DXGI_FORMAT_R32G32_SINT,
    DXGI_FORMAT_R16G16_TYPELESS,
    DXGI_FORMAT_R16G16_FLOAT,
    DXGI_FORMAT_R16G16_UNORM,
    DXGI_FORMAT_R16G16_UINT,
    DXGI_FORMAT_R16G16_SNORM,
    DXGI_FORMAT_R16G16_SINT,
    DXGI_FORMAT_R8G8_TYPELESS,
    DXGI_FORMAT_R8G8_UNORM,
    DXGI_FORMAT_R8G8_UINT,
    DXGI_FORMAT_R8G8_SNORM,
    DXGI_FORMAT_R8G8_SINT,
    //1 chanel
    DXGI_FORMAT_R32_TYPELESS,
    DXGI_FORMAT_R32_FLOAT,
    DXGI_FORMAT_R32_UINT,
    DXGI_FORMAT_R32_SINT,
    DXGI_FORMAT_R16_TYPELESS,
    DXGI_FORMAT_R16_FLOAT,
    DXGI_FORMAT_R16_UNORM,
    DXGI_FORMAT_R16_UINT,
    DXGI_FORMAT_R16_SNORM,
    DXGI_FORMAT_R16_SINT,
    DXGI_FORMAT_R8_TYPELESS,
    DXGI_FORMAT_R8_UNORM,
    DXGI_FORMAT_R8_UINT,
    DXGI_FORMAT_R8_SNORM,
    DXGI_FORMAT_R8_SINT,
    DXGI_FORMAT_A8_UNORM,
};

constexpr DXGI_FORMAT ToD3D_DepthFormat[] = {
    DXGI_FORMAT_UNKNOWN,
    DXGI_FORMAT_D24_UNORM_S8_UINT,
    DXGI_FORMAT_D24_UNORM_S8_UINT,
    DXGI_FORMAT_D16_UNORM,
    DXGI_FORMAT_D32_FLOAT,
    DXGI_FORMAT_D32_FLOAT_S8X24_UINT,
};

UINT ToD3D11ResourceBinDings(uint32_t bindings)
{
    UINT result = 0;
    constexpr std::pair<uint32_t, D3D11_BIND_FLAG> BindFlags[] = {
        {to_underlying(EBindFlags::BIND_VERTEX_BUFFER),D3D11_BIND_VERTEX_BUFFER},
        {to_underlying(EBindFlags::BIND_INDEX_BUFFER),D3D11_BIND_INDEX_BUFFER},
        {to_underlying(EBindFlags::BIND_CONSTANT_BUFFER),D3D11_BIND_CONSTANT_BUFFER},
        {to_underlying(EBindFlags::BIND_SHADER_RESOURCE),D3D11_BIND_SHADER_RESOURCE},
        {to_underlying(EBindFlags::BIND_STREAM_OUTPUT),D3D11_BIND_STREAM_OUTPUT},
        {to_underlying(EBindFlags::BIND_RENDER_TARGET),D3D11_BIND_RENDER_TARGET},
        {to_underlying(EBindFlags::BIND_DEPTH_STENCIL),D3D11_BIND_DEPTH_STENCIL},
        {to_underlying(EBindFlags::BIND_UNORDERED_ACCESS),D3D11_BIND_UNORDERED_ACCESS},
    };
    for (auto& flag : BindFlags)
    {
        if (bindings & flag.first)
            result |= flag.second;
    }
    return result;
}

IRenderDevice::IResource* RenderDeviceDX11::CreateResource(const GpuResource& ResourceDesc)
{
    switch (ResourceDesc.resourceDescription.Dimension)
    {
    case EResourceDimension::RESOURCE_DIMENSION_TEXTURE2D:
    {
        ID3D11Texture2D* result;

        D3D11_TEXTURE2D_DESC dx11desc;

        auto& desc = ResourceDesc.resourceDescription;

        /* Initialize descriptor */
        dx11desc.Width = desc.Width;
        dx11desc.Height = desc.Height;
        dx11desc.MipLevels = 1;
        dx11desc.ArraySize = desc.DepthOrArraySize;
        dx11desc.Format = ToD3D_TextureFormat[to_underlying(desc.Format)];
        dx11desc.SampleDesc.Count = 1;
        dx11desc.SampleDesc.Quality = 0;
        dx11desc.Usage = D3D11_USAGE_DEFAULT;
        dx11desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | ToD3D11ResourceBinDings(ResourceDesc.resourceBindings);
        dx11desc.CPUAccessFlags = 0;
        dx11desc.MiscFlags = 0;

        //   ResourceDesc.resourceBindings;
        //if (isRenderTarget)
        // {
        //    desc.BindFlags |= D3D11_BIND_RENDER_TARGET;
        //   desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
        // }

        /* Create the texture */
        GFX_THROW_INFO(device->CreateTexture2D(
            &dx11desc,
            NULL,
            &result
        ));

        return reinterpret_cast<IRenderDevice::IResource*>(result);
    }
    case EResourceDimension::RESOURCE_DIMENSION_BUFFER:
    {
        ID3D11Buffer* result;

        D3D11_BUFFER_DESC dx11desc;

        auto& desc = ResourceDesc.resourceDescription;

        /* Initialize descriptor */
        dx11desc.ByteWidth = desc.Width;
        dx11desc.Usage = D3D11_USAGE_DEFAULT;
        dx11desc.BindFlags = ToD3D11ResourceBinDings(ResourceDesc.resourceBindings);
        dx11desc.CPUAccessFlags = 0;
        dx11desc.MiscFlags = 0;

        //   ResourceDesc.resourceBindings;
        //if (isRenderTarget)
        // {
        //    desc.BindFlags |= D3D11_BIND_RENDER_TARGET;
        //   desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
        // }

        /* Create the texture */
        GFX_THROW_INFO(device->CreateBuffer(
            &dx11desc,
            NULL,
            &result
        ));

        return reinterpret_cast<IRenderDevice::IResource*>(result);
    }
    }
    return nullptr;
}

void RenderDeviceDX11::DestroyResource(IResource* resource)
{
    IUnknown* d3d11resource = reinterpret_cast<IUnknown*>(resource);
    d3d11resource->Release();
}


D3D11_DEPTH_STENCIL_VIEW_DESC ToD3D11DepthStencilView(const DepthStencilViewDesc& desc)
{
    D3D11_DEPTH_STENCIL_VIEW_DESC result;
    result.Flags = to_underlying(desc.Flag);
    result.Format = ToD3D_DepthFormat[to_underlying(desc.Format)];
    result.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    result.Texture2D.MipSlice = 0;
    return result;
}


constexpr D3D_SRV_DIMENSION ToD3D_ShaderViewDimension[] = {

    D3D_SRV_DIMENSION_UNKNOWN,
    D3D_SRV_DIMENSION_TEXTURE1D,
    D3D_SRV_DIMENSION_TEXTURE1DARRAY,
    D3D_SRV_DIMENSION_TEXTURE2D,
    D3D_SRV_DIMENSION_TEXTURE2DARRAY,
    D3D_SRV_DIMENSION_TEXTURE3D,
    D3D_SRV_DIMENSION_TEXTURECUBE,
    D3D_SRV_DIMENSION_TEXTURECUBEARRAY,
};

D3D11_SHADER_RESOURCE_VIEW_DESC ToD3D11ShaderView(const ShaderResourceViewDesc& desc)
{
    D3D11_SHADER_RESOURCE_VIEW_DESC result;
    result.Format = ToD3D_TextureFormat[to_underlying(desc.Format)];
    result.ViewDimension = ToD3D_ShaderViewDimension[to_underlying(desc.Dimension)];
    result.Texture2D.MipLevels = 1;
    result.Texture2D.MostDetailedMip = 0;
    return result;
}

IRenderDevice::IResourceView* RenderDeviceDX11::CreateResourceView(const GpuResourceView& desc, const GpuResource& ResourceDesc)
{
    switch (desc.type)
    {
    case GpuResourceView::EViewType::CB:
    {
        ConstBufferViewD3D11* result = new ConstBufferViewD3D11(desc.cbViewDescription, reinterpret_cast<ID3D11Buffer*>(ResourceDesc.resource));
        return reinterpret_cast<IResourceView*>(result);
        break;
    }
    case GpuResourceView::EViewType::VB:
    {
        VertexBufferViewD3D11* result = new VertexBufferViewD3D11(desc.vbViewDescription, reinterpret_cast<ID3D11Buffer*>(ResourceDesc.resource));
        return reinterpret_cast<IResourceView*>(result);
        break;
    }
    case GpuResourceView::EViewType::IB:
    {
        IndexBufferViewD3D11* result = new IndexBufferViewD3D11(desc.ibViewDescription, reinterpret_cast<ID3D11Buffer*>(ResourceDesc.resource));
        return reinterpret_cast<IResourceView*>(result);
        break;
    }
    case GpuResourceView::EViewType::DB:
    {
        ID3D11DepthStencilView* result;
        auto d3d11desc = ToD3D11DepthStencilView(desc.dbViewDescription);
        device->CreateDepthStencilView(
            reinterpret_cast<ID3D11Resource*>(ResourceDesc.resource),
            &d3d11desc,
            &result
        );
        return reinterpret_cast<IResourceView*>(result);
        break;
    }
    case GpuResourceView::EViewType::RT:
    {
        return nullptr;
        break;
    }
    case GpuResourceView::EViewType::SR:
    {
        ID3D11ShaderResourceView* result;
        if (desc.srViewDescription.MakeDefault)
        {
            device->CreateShaderResourceView(
                reinterpret_cast<ID3D11Resource*>(ResourceDesc.resource),
                nullptr,
                &result
            );
        }
        else
        {
            auto d3d11desc = ToD3D11ShaderView(desc.srViewDescription);
            device->CreateShaderResourceView(
                reinterpret_cast<ID3D11Resource*>(ResourceDesc.resource),
                &d3d11desc,
                &result
            );
        }

        return reinterpret_cast<IResourceView*>(result);
        break;
    }
    case GpuResourceView::EViewType::UA:
    {
        return nullptr;
        break;
    }
    }
    return nullptr;
}


IRenderDevice::IShader* RenderDeviceDX11::CreateShader(const ShaderDesc& desc)
{
    ID3D11DeviceChild* result = nullptr;
    switch (desc.type)
    {
    case EShaderType::HULL_SHADER:
    {
        ID3D11HullShader* hs;
        device->CreateHullShader(desc.bytecode, desc.byteCodeSize, nullptr, &hs);
        result = hs;
        break;
    }
    case EShaderType::PIXEL_SHADER:
    {
        ID3D11PixelShader* ps;
        device->CreatePixelShader(desc.bytecode, desc.byteCodeSize, nullptr, &ps);
        result = ps;
        break;
    }
    case EShaderType::DOMAIN_SHADER:
    {
        ID3D11DomainShader* ds;
        device->CreateDomainShader(desc.bytecode, desc.byteCodeSize, nullptr, &ds);
        result = ds;
        break;
    }
    case EShaderType::VERTEX_SHADER:
    {
        ID3D11VertexShader* vs;
        device->CreateVertexShader(desc.bytecode, desc.byteCodeSize, nullptr, &vs);
        result = vs;
        break;
    }
    case EShaderType::COMPUTE_SHADER:
    {
        ID3D11ComputeShader* cs;
        device->CreateComputeShader(desc.bytecode, desc.byteCodeSize, nullptr, &cs);
        result = cs;
        break;
    }
    case EShaderType::GEOMETRY_SHADER:
    {
        ID3D11GeometryShader* gs;
        device->CreateGeometryShader(desc.bytecode, desc.byteCodeSize, nullptr, &gs);
        result = gs;
        break;
    }
    default:
    {
        break;
    };
    }
    // GFX_THROW_INFO(
    //           );
    return reinterpret_cast<IRenderDevice::IShader*>(result);
}

std::vector<D3D11_INPUT_ELEMENT_DESC> ToD3D11(const InputAssemblerDeclarationDesc& desc)
{
    std::vector<D3D11_INPUT_ELEMENT_DESC> result(desc.InputElementDescs.size());
    for (int i = 0; i < desc.InputElementDescs.size(); i++)
    {
        result[i].Format = ToD3D_TextureFormat[to_underlying(desc.InputElementDescs[i].Format)];
        result[i].InputSlot = desc.InputElementDescs[i].InputSlot;
        result[i].SemanticIndex = desc.InputElementDescs[i].SemanticIndex;
        result[i].SemanticName = desc.InputElementDescs[i].SemanticName;
        result[i].AlignedByteOffset = desc.InputElementDescs[i].AlignedByteOffset;
        result[i].InputSlotClass = D3D11_INPUT_CLASSIFICATION(to_underlying(desc.InputElementDescs[i].InputSlotClass));
        result[i].InstanceDataStepRate = desc.InputElementDescs[i].InstanceDataStepRate;
    }
    return result;
}

IRenderDevice::IInputLayout* RenderDeviceDX11::CreateInputLayout(const InputAssemblerDeclarationDesc& desc, const ShaderDesc& Shader)
{
    auto d3d11InputDesc = ToD3D11(desc);
    ID3D11InputLayout* inputLayout;

    device->CreateInputLayout(
        d3d11InputDesc.data(),
        d3d11InputDesc.size(),
        Shader.bytecode,
        Shader.byteCodeSize,
        &inputLayout);

    return reinterpret_cast<IRenderDevice::IInputLayout*>(inputLayout);
}
void RenderDeviceDX11::SetResourceData(const GpuResource& resource, uint16_t dstSubresource, const UBox& rect, const void* pSrcData, int32_t srcRowPitch,
    int32_t srcDepthPitch)
{
    D3D11_BOX dstBox;

    //int32_t blockSize = Texture_GetBlockSize(d3dTexture->format);

    ///if (blockSize > 1) {
    //	w = (w + blockSize - 1) & ~(blockSize - 1);
    //	h = (h + blockSize - 1) & ~(blockSize - 1);
    //}
    if (resource.resourceBindings && to_underlying(EBindFlags::BIND_CONSTANT_BUFFER))
    {
        GFX_THROW_INFO_ONLY(context->UpdateSubresource(
            reinterpret_cast<ID3D11Resource*>(resource.resource),
            dstSubresource,
            nullptr,
            pSrcData,
            srcRowPitch,
            srcDepthPitch
        ));
    }
    dstBox.left = rect.Left;
    dstBox.top = rect.Top;
    dstBox.front = rect.Front;
    dstBox.right = rect.Right;
    dstBox.bottom = rect.Bottom;
    dstBox.back = rect.Back;


    //SDL_LockMutex(renderer->ctxLock);
}

bool ToD3D11Viewports(const Compressed::ViewportDesc viewports[], D3D11_VIEWPORT d3d11viewports[8], uint8_t num)
{
    bool result = false;
    for (int i = 0; i < num; i++)
    {
        if (((D3D11_VIEWPORT*)viewports)[i] != d3d11viewports[i])
        {
            d3d11viewports[i] = ((D3D11_VIEWPORT*)viewports)[i];
            result = true;
        }
    }
    return result;
}

void RenderDeviceDX11::SetupViewports(const Compressed::ViewportDesc viewports[], uint8_t num)
{
    static D3D11_VIEWPORT d3d11viewports[8];
    if (ToD3D11Viewports(viewports, d3d11viewports, num))
    {
        GFX_THROW_INFO_ONLY(context->RSSetViewports(num, d3d11viewports));
    }
}

ID3D11BlendState* RenderDeviceDX11::FetchBlendState(const Compressed::CoreBlendDesc& blendState)
{
    if (hashBS.size() == 0)
    {
        ID3D11BlendState* bs;
        CD3D11_BLEND_DESC desc{CD3D11_DEFAULT()};

        device->CreateBlendState(&desc, &bs);
        hashBS.insert({0,bs});
    }
    return hashBS[0];
}

void RenderDeviceDX11::SetupBlendState(const Compressed::CoreBlendDesc& blendState)
{
    static FLOAT BlendFactor[4];
    blendFactor = {
        blendState.desc.BlendFactor[0],
        blendState.desc.BlendFactor[1],
        blendState.desc.BlendFactor[2],
        blendState.desc.BlendFactor[3]
    };

    auto bs = FetchBlendState(blendState);
    context->OMSetBlendState(bs, blendFactor.Color, blendState.desc.SampleMask);
}

ID3D11DepthStencilState* RenderDeviceDX11::FetchDepthStencilState(const Compressed::DepthStencilStateDesc& depthStencilState)
{
    if (hashDSS.size() == 0)
    {
        ID3D11DepthStencilState* bs;
        CD3D11_DEPTH_STENCIL_DESC desc{CD3D11_DEFAULT()};

        device->CreateDepthStencilState(&desc, &bs);
        hashDSS.insert({0,bs});
    }
    return hashDSS[0];
}

void RenderDeviceDX11::SetupDepthStencilState(const Compressed::DepthStencilStateDesc& depthStencilState)
{
    auto ds = FetchDepthStencilState(depthStencilState);
    context->OMSetDepthStencilState(ds, 0);
}

ID3D11RasterizerState* RenderDeviceDX11::FetchRasterizerState(const Compressed::RasterizerStateDesc& rasterizerState)
{
    if (hashRS.size() == 0)
    {
        ID3D11RasterizerState* bs;
        CD3D11_RASTERIZER_DESC desc{CD3D11_DEFAULT()};

        device->CreateRasterizerState(&desc, &bs);
        hashRS.insert({0,bs});
    }
    return hashRS[0];
}


void RenderDeviceDX11::SetupRasterizerState(const Compressed::RasterizerStateDesc& rasterizerState)
{
    auto rs = FetchRasterizerState(rasterizerState);
    context->RSSetState(rs);
}

D3D11_FILTER ToD3D11SamplerFilter[] = {
    D3D11_FILTER_MIN_MAG_MIP_POINT,
    D3D11_FILTER_MIN_MAG_MIP_POINT,
    D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR,
    D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT,
    D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR,
    D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT,
    D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR,
    D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT,
    D3D11_FILTER_MIN_MAG_MIP_LINEAR,
    D3D11_FILTER_ANISOTROPIC,
    D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT,
    D3D11_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR,
    D3D11_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT,
    D3D11_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR,
    D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT,
    D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR,
    D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT,
    D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR,
    D3D11_FILTER_COMPARISON_ANISOTROPIC,
    D3D11_FILTER_MINIMUM_MIN_MAG_MIP_POINT,
    D3D11_FILTER_MINIMUM_MIN_MAG_POINT_MIP_LINEAR,
    D3D11_FILTER_MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT,
    D3D11_FILTER_MINIMUM_MIN_POINT_MAG_MIP_LINEAR,
    D3D11_FILTER_MINIMUM_MIN_LINEAR_MAG_MIP_POINT,
    D3D11_FILTER_MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR,
    D3D11_FILTER_MINIMUM_MIN_MAG_LINEAR_MIP_POINT,
    D3D11_FILTER_MINIMUM_MIN_MAG_MIP_LINEAR,
    D3D11_FILTER_MINIMUM_ANISOTROPIC,
    D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_POINT,
    D3D11_FILTER_MAXIMUM_MIN_MAG_POINT_MIP_LINEAR,
    D3D11_FILTER_MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT,
    D3D11_FILTER_MAXIMUM_MIN_POINT_MAG_MIP_LINEAR,
    D3D11_FILTER_MAXIMUM_MIN_LINEAR_MAG_MIP_POINT,
    D3D11_FILTER_MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR,
    D3D11_FILTER_MAXIMUM_MIN_MAG_LINEAR_MIP_POINT,
    D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR,
    D3D11_FILTER_MAXIMUM_ANISOTROPIC
};

ID3D11SamplerState* RenderDeviceDX11::FetchSamplerState(const Compressed::SamplerStateDesc& state)
{
    if (hashSS.contains(state.Data))
        return hashSS[state.Data];

    ID3D11SamplerState* istate;

    CD3D11_SAMPLER_DESC desc;
    assert(state.Fields.Filter != 0);
    desc.Filter = ToD3D11SamplerFilter[state.Fields.Filter];
    desc.MaxAnisotropy = state.Fields.MaxAnisotropy;
    desc.MaxLOD = state.MaxLOD;
    desc.MinLOD = state.MinLOD;

    device->CreateSamplerState(&desc, &istate);
    hashSS.insert({state.Data,istate});
    return hashSS[state.Data];
}


void RenderDeviceDX11::SetupSamplers(const Compressed::SamplerStateDesc samplers[], uint8_t num)
{
    ID3D11SamplerState* Samplers[16] = {};
    for (int i = 0; i < num; i++)
    {
        Samplers[i] = FetchSamplerState((samplers[i]));
    }
    context->CSSetSamplers(0, num, Samplers);
    context->PSSetSamplers(0, num, Samplers);
    context->GSSetSamplers(0, num, Samplers);
    context->DSSetSamplers(0, num, Samplers);
    context->HSSetSamplers(0, num, Samplers);
    context->VSSetSamplers(0, num, Samplers);
}

D3D11_PRIMITIVE_TOPOLOGY ToD3DPT[] = {
    D3D10_PRIMITIVE_TOPOLOGY_UNDEFINED,
    D3D11_PRIMITIVE_TOPOLOGY_POINTLIST,
    D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
    D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP,
    D3D10_PRIMITIVE_TOPOLOGY_UNDEFINED,
    D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
    D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
    D3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ,
    D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ,
    D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ,
    D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ,
    D3D11_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST,
    D3D11_PRIMITIVE_TOPOLOGY_2_CONTROL_POINT_PATCHLIST,
    D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST,
    D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST,
};

void RenderDeviceDX11::SetupPrimitiveTopology(const EPrimitiveTopology topology)
{
    context->IASetPrimitiveTopology(ToD3DPT[to_underlying(topology)]);
}

void RenderDeviceDX11::SetupVertexBuffer(const IVertexBufferView* vertexBuffers[], uint8_t num)
{
    this->vertexBuffers.resize(32);
    this->vertexBufferOffsets.resize(32);
    this->vertexBufferStrides.resize(32);
    for (int i = 0; i < num; i++)
    {
        const VertexBufferViewD3D11* vb = reinterpret_cast<const VertexBufferViewD3D11*>(vertexBuffers[i]);
        if (vb != nullptr)
        {
            this->vertexBuffers[i] = vb->vertexBuffer;
            vertexBufferOffsets[i] = vb->Offset;
            vertexBufferStrides[i] = vb->Stride;
        }
        else
        {
            this->vertexBuffers[i] = nullptr;
            vertexBufferOffsets[i] = 0;
            vertexBufferStrides[i] = 0;
        }
    }
    context->IASetVertexBuffers(0, num, this->vertexBuffers.data(), vertexBufferStrides.data(), vertexBufferOffsets.data());
}

void RenderDeviceDX11::SetupIndexBuffer(const IIndexBufferView* indices)
{
    const IndexBufferViewD3D11* indexBuffer = reinterpret_cast<const IndexBufferViewD3D11*>(indices);
    context->IASetIndexBuffer(indexBuffer->indexBuffer, indexBuffer->format, 0);
}

void RenderDeviceDX11::SetupTextures(IResourceView* textures[], uint8_t num)
{
    for (int i = 0; i < num; i++)
    {
        ID3D11ShaderResourceView* sh = const_cast<ID3D11ShaderResourceView*>(reinterpret_cast<const ID3D11ShaderResourceView*>(textures[i]));
        Textures[i] = sh;
    }
    context->CSSetShaderResources(0, num, Textures.data());
    context->PSSetShaderResources(0, num, Textures.data());
    context->GSSetShaderResources(0, num, Textures.data());
    context->DSSetShaderResources(0, num, Textures.data());
    context->HSSetShaderResources(0, num, Textures.data());
    context->VSSetShaderResources(0, num, Textures.data());
}

void RenderDeviceDX11::SetupRenderTargets(const IRenderTargetView* renderTargets[], int32_t num, IDepthStencilView* depthStencilBuffer)
{
    for (int i = 0; i < num; i++)
    {
        renderTargetViews[i] = renderTargets[i] == nullptr ? swapchainRTView : ((ID3D11RenderTargetView*)renderTargets[i]);
    }
    for (int i = num; i < MAX_RENDERTARGET_ATTACHMENTS; i++)
    {
        renderTargetViews[i] = nullptr;
    }
    this->depthStencilBuffer = (ID3D11DepthStencilView*)depthStencilBuffer;

    context->OMSetRenderTargets(MAX_RENDERTARGET_ATTACHMENTS, renderTargetViews.data(), this->depthStencilBuffer);
}



void RenderDeviceDX11::SetupShader(IShader* shader, EShaderType type)
{
    switch (type)
    {
    case EShaderType::COMPUTE_SHADER:
    {
        context->CSSetShader((ID3D11ComputeShader*)shader, nullptr, 0);
        break;
    }
    case EShaderType::HULL_SHADER:
    {
        context->HSSetShader((ID3D11HullShader*)shader, nullptr, 0);
        break;
    }
    case EShaderType::PIXEL_SHADER:
    {
        context->PSSetShader((ID3D11PixelShader*)shader, nullptr, 0);
        break;
    }
    case EShaderType::DOMAIN_SHADER:
    {
        context->DSSetShader((ID3D11DomainShader*)shader, nullptr, 0);
        break;
    }
    case EShaderType::VERTEX_SHADER:
    {
        context->VSSetShader((ID3D11VertexShader*)shader, nullptr, 0);
        break;
    }
    case EShaderType::GEOMETRY_SHADER:
    {
        context->GSSetShader((ID3D11GeometryShader*)shader, nullptr, 0);
        break;
    }
    }
}

void RenderDeviceDX11::SetupConstBuffers(IConstBufferView* constBuffers[], uint8_t num)
{
    static ID3D11Buffer* buffers[32];
    for (int i = 0; i < num; i++)
    {
        buffers[i] = ((ConstBufferViewD3D11**)constBuffers)[i] ? ((ConstBufferViewD3D11**)constBuffers)[i]->constBuffer : nullptr;
    }
    for (int i = num; i < 32; i++)
    {
        buffers[i] = nullptr;
    }
    context->CSSetConstantBuffers(0, num, buffers);
    context->PSSetConstantBuffers(0, num, buffers);
    context->GSSetConstantBuffers(0, num, buffers);
    context->DSSetConstantBuffers(0, num, buffers);
    context->HSSetConstantBuffers(0, num, buffers);
    context->VSSetConstantBuffers(0, num, buffers);
}
void RenderDeviceDX11::SetupInputLayout(IInputLayout* layout)
{
    context->IASetInputLayout((ID3D11InputLayout*)layout);
}

void RenderDeviceDX11::Draw(DrawCall call)
{
    switch (call.type)
    {
    case EDrawCallType::DRAW_INDEXED:
    {
        context->DrawIndexed(call.get<0>(), call.get<1>(), call.get<2>());
        break;
    }
    case EDrawCallType::DRAW:
    {
        context->Draw(call.get<0>(), call.get<1>());
        break;
    }
    case EDrawCallType::DRAW_INDEXED_INSTANCED:
    {
        context->DrawIndexedInstanced(call.get<0>(), call.get<1>(), call.get<2>(),call.get<3>(),call.get<4>());
        break;
    }
    }
}
void RenderDeviceDX11::Present()
{
    GFX_THROW_INFO(swapchain->Present(syncInterval, 0));
}
