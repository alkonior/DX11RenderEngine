#include "RenderDeviceDX11.h"

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
    vertexTextures.resize(MAX_TEXTURE_ATTACHMENTS, nullptr);
    vertexSamplers.resize(MAX_SAMPLER_ATTACHMENTS, nullptr);

    pixelTextures.resize(MAX_TEXTURE_ATTACHMENTS, nullptr);
    pixelSamplers.resize(MAX_TEXTURE_ATTACHMENTS, nullptr);

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
    }
    return nullptr;
}

void RenderDeviceDX11::DestroyResource(IResource* resource)
{
    ID3D11Texture2D* d3d11resource = reinterpret_cast<ID3D11Texture2D*>(resource);
    d3d11resource->Release();
}

IRenderDevice::IResourceView* RenderDeviceDX11::CreateResourceView(const GpuResourceView& desc)
{
    switch (desc.type)
    {
    case GpuResourceView::EViewType::CB:
        {
            return nullptr;
            break;
        }
    case GpuResourceView::EViewType::DB:
        {
            return nullptr;
            break;
        }
    case GpuResourceView::EViewType::IB:
        {
            return nullptr;
            break;
        }
    case GpuResourceView::EViewType::RT:
        {
            return nullptr;
            break;
        }
    case GpuResourceView::EViewType::VB:
        {
            return nullptr;
            break;
        }
    case GpuResourceView::EViewType::SR:
        {
            return nullptr;
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
        }
        ;
    }
    // GFX_THROW_INFO(
    //           );
    return reinterpret_cast<IRenderDevice::IShader*>(result);
}

IRenderDevice::IInputLayout* RenderDeviceDX11::CreateInputLayout(const InputAssemblerDeclarationDesc& desc)
{
    return &inputLayout;
}
