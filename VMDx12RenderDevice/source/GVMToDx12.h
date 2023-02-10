#include "winHandlerdx12.h"

using namespace GVM;

static UINT CalcConstantBufferByteSize(UINT byteSize)
{
    return (byteSize + 255) & ~255;
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

#define ZEROS(r) std::memset(&r, 0, sizeof(r));

D3D12_DEPTH_STENCIL_VIEW_DESC inline ToD3D12DepthStencilView(const DepthStencilViewDesc& desc)
{
    D3D12_DEPTH_STENCIL_VIEW_DESC result;
    ZEROS(result)
    result.Format = ToD3D_DepthFormat[to_underlying(desc.Format)];
    result.Flags = (D3D12_DSV_FLAGS)to_underlying(desc.Flag);
    result.Format = ToD3D_DepthFormat[to_underlying(desc.Format)];
    result.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    result.Texture2D.MipSlice = 0;
    return result;
}


constexpr D3D12_SRV_DIMENSION ToD3D_ShaderViewDimension[] = {

    D3D12_SRV_DIMENSION_UNKNOWN,
    D3D12_SRV_DIMENSION_TEXTURE1D,
    D3D12_SRV_DIMENSION_TEXTURE1DARRAY,
    D3D12_SRV_DIMENSION_TEXTURE2D,
    D3D12_SRV_DIMENSION_TEXTURE2DARRAY,
    D3D12_SRV_DIMENSION_TEXTURE3D,
    D3D12_SRV_DIMENSION_TEXTURECUBE,
    D3D12_SRV_DIMENSION_TEXTURECUBEARRAY,
};

constexpr D3D12_RTV_DIMENSION ToD3D_RTViewDimension[] = {
    D3D12_RTV_DIMENSION_UNKNOWN,
    D3D12_RTV_DIMENSION_TEXTURE1D,
    D3D12_RTV_DIMENSION_TEXTURE1DARRAY,
    D3D12_RTV_DIMENSION_TEXTURE2D,
    D3D12_RTV_DIMENSION_TEXTURE2DARRAY,
    D3D12_RTV_DIMENSION_TEXTURE3D,
};


constexpr D3D12_UAV_DIMENSION ToD3D_UAViewDimension[] = {
    D3D12_UAV_DIMENSION_UNKNOWN,
    D3D12_UAV_DIMENSION_TEXTURE1D,
    D3D12_UAV_DIMENSION_TEXTURE1DARRAY,
    D3D12_UAV_DIMENSION_TEXTURE2D,
    D3D12_UAV_DIMENSION_TEXTURE2DARRAY,
    D3D12_UAV_DIMENSION_TEXTURE3D,
};


D3D12_SHADER_RESOURCE_VIEW_DESC inline ToD3D12ShaderView(const ShaderResourceViewDesc& desc)
{
    D3D12_SHADER_RESOURCE_VIEW_DESC result;
    ZEROS(result)
    result.Format = ToD3D_TextureFormat[to_underlying(desc.Format)];
    result.ViewDimension = ToD3D_ShaderViewDimension[to_underlying(desc.Dimension)];
    result.Texture2D.MipLevels = 1;
    result.Texture2D.MostDetailedMip = 0;
    result.Texture2D.PlaneSlice = 0;
    result.Texture2D.ResourceMinLODClamp = 0;
    result.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    return result;
}

D3D12_RENDER_TARGET_VIEW_DESC inline ToD3D12RTView(const RenderTargetViewDesc& desc)
{
    D3D12_RENDER_TARGET_VIEW_DESC result;
    ZEROS(result)
    result.Format = ToD3D_TextureFormat[to_underlying(desc.Format)];
    result.ViewDimension = ToD3D_RTViewDimension[to_underlying(desc.Dimension)];
    result.Texture2D.MipSlice = 0;

    return result;
}

D3D12_UNORDERED_ACCESS_VIEW_DESC inline ToD3D12UAView(const UATargetViewDesc& desc)
{
    D3D12_UNORDERED_ACCESS_VIEW_DESC result;
    ZEROS(result)
    result.Format = ToD3D_TextureFormat[to_underlying(desc.Format)];
    result.ViewDimension = ToD3D_UAViewDimension[to_underlying(desc.Dimension)];
    result.Texture2D.MipSlice = 0;
    return result;
}


std::vector<D3D12_INPUT_ELEMENT_DESC> inline ToD3D11(const InputAssemblerDeclarationDesc& desc)
{
    std::vector<D3D12_INPUT_ELEMENT_DESC> result(desc.InputElementDescs.size());
    for (int i = 0; i < desc.InputElementDescs.size(); i++)
    {
        result[i].Format = ToD3D_TextureFormat[to_underlying(desc.InputElementDescs[i].Format)];
        result[i].InputSlot = desc.InputElementDescs[i].InputSlot;
        result[i].SemanticIndex = desc.InputElementDescs[i].SemanticIndex;
        result[i].SemanticName = desc.InputElementDescs[i].SemanticName;
        result[i].AlignedByteOffset = desc.InputElementDescs[i].AlignedByteOffset;
        result[i].InputSlotClass = D3D12_INPUT_CLASSIFICATION(to_underlying(desc.InputElementDescs[i].InputSlotClass));
        result[i].InstanceDataStepRate = desc.InputElementDescs[i].InstanceDataStepRate;
    }
    return result;
}

bool inline ToD3D12Viewports(const Compressed::ViewportDesc viewports[], D3D12_VIEWPORT d3d11viewports[20], D3D12_RECT scissorsRect[20], uint8_t num)
{
    bool result = false;
    for (int i = 0; i < num; i++)
    {
        if (((D3D12_VIEWPORT*)viewports)[i] != d3d11viewports[i])
        {
            d3d11viewports[i] = ((D3D12_VIEWPORT*)viewports)[i];
            scissorsRect[i].bottom = d3d11viewports[i].Height;
            scissorsRect[i].left = d3d11viewports[i].TopLeftX;
            scissorsRect[i].right = d3d11viewports[i].Width;
            scissorsRect[i].top = d3d11viewports[i].TopLeftY;
            result = true;
        }
        assert(-32768.000000 <= d3d11viewports[i].TopLeftX &&
            -32768.000000 <= d3d11viewports[i].TopLeftY &&
            -32768.000000 <= (d3d11viewports[i].TopLeftX+d3d11viewports[i].Width) &&
            (d3d11viewports[i].TopLeftX+d3d11viewports[i].Width) <= 32767.000000 &&
            (d3d11viewports[i].TopLeftY+d3d11viewports[i].Height) <= 32767.000000 &&
            0.000000 <= d3d11viewports[i].MinDepth &&
            d3d11viewports[i].MaxDepth <= 1.000000 &&
            d3d11viewports[i].MinDepth <= d3d11viewports[i].MaxDepth
        );
    }
    return result;
}

D3D12_BLEND_DESC inline ToD3D12Blend(const Compressed::CoreBlendDesc& blendState)
{
    D3D12_BLEND_DESC desc{D3D12_BLEND_DESC()};
    ZEROS(desc);
    desc.AlphaToCoverageEnable = 0;
    desc.IndependentBlendEnable = 0;

    for (int i = 0; i < 8; i++)
    {
        auto& tbs = blendState.BlendStates[i];
        desc.RenderTarget[i].BlendEnable = tbs.Fields.BlendEnable &&
            !(
                tbs.Fields.SrcBlend == D3D12_BLEND_ONE &&
                tbs.Fields.DestBlend == D3D12_BLEND_ZERO &&
                tbs.Fields.SrcBlendAlpha == D3D12_BLEND_ONE &&
                tbs.Fields.DestBlendAlpha == D3D12_BLEND_ZERO
            );
        if (desc.RenderTarget[i].BlendEnable)
        {
            desc.RenderTarget[i].BlendOp = D3D12_BLEND_OP((tbs.Fields.BlendOp));
            desc.RenderTarget[i].BlendOpAlpha = D3D12_BLEND_OP((tbs.Fields.BlendOpAlpha));
            desc.RenderTarget[i].DestBlend = D3D12_BLEND((tbs.Fields.DestBlend));
            desc.RenderTarget[i].DestBlendAlpha = D3D12_BLEND((tbs.Fields.DestBlendAlpha));
            desc.RenderTarget[i].SrcBlend = D3D12_BLEND((tbs.Fields.SrcBlend));
            desc.RenderTarget[i].SrcBlendAlpha = D3D12_BLEND((tbs.Fields.SrcBlendAlpha));
        }
    }
    return desc;
}

constexpr size_t BitsPerPixel(DXGI_FORMAT fmt)
{
    switch (static_cast<int>(fmt))
    {
    case DXGI_FORMAT_R32G32B32A32_TYPELESS:
    case DXGI_FORMAT_R32G32B32A32_FLOAT:
    case DXGI_FORMAT_R32G32B32A32_UINT:
    case DXGI_FORMAT_R32G32B32A32_SINT: return 128;

    case DXGI_FORMAT_R32G32B32_TYPELESS:
    case DXGI_FORMAT_R32G32B32_FLOAT:
    case DXGI_FORMAT_R32G32B32_UINT:
    case DXGI_FORMAT_R32G32B32_SINT: return 96;

    case DXGI_FORMAT_R16G16B16A16_TYPELESS:
    case DXGI_FORMAT_R16G16B16A16_FLOAT:
    case DXGI_FORMAT_R16G16B16A16_UNORM:
    case DXGI_FORMAT_R16G16B16A16_UINT:
    case DXGI_FORMAT_R16G16B16A16_SNORM:
    case DXGI_FORMAT_R16G16B16A16_SINT:
    case DXGI_FORMAT_R32G32_TYPELESS:
    case DXGI_FORMAT_R32G32_FLOAT:
    case DXGI_FORMAT_R32G32_UINT:
    case DXGI_FORMAT_R32G32_SINT:
    case DXGI_FORMAT_R32G8X24_TYPELESS:
    case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
    case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
    case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
    case DXGI_FORMAT_Y416:
    case DXGI_FORMAT_Y210:
    case DXGI_FORMAT_Y216: return 64;

    case DXGI_FORMAT_R10G10B10A2_TYPELESS:
    case DXGI_FORMAT_R10G10B10A2_UNORM:
    case DXGI_FORMAT_R10G10B10A2_UINT:
    case DXGI_FORMAT_R11G11B10_FLOAT:
    case DXGI_FORMAT_R8G8B8A8_TYPELESS:
    case DXGI_FORMAT_R8G8B8A8_UNORM:
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
    case DXGI_FORMAT_R8G8B8A8_UINT:
    case DXGI_FORMAT_R8G8B8A8_SNORM:
    case DXGI_FORMAT_R8G8B8A8_SINT:
    case DXGI_FORMAT_R16G16_TYPELESS:
    case DXGI_FORMAT_R16G16_FLOAT:
    case DXGI_FORMAT_R16G16_UNORM:
    case DXGI_FORMAT_R16G16_UINT:
    case DXGI_FORMAT_R16G16_SNORM:
    case DXGI_FORMAT_R16G16_SINT:
    case DXGI_FORMAT_R32_TYPELESS:
    case DXGI_FORMAT_D32_FLOAT:
    case DXGI_FORMAT_R32_FLOAT:
    case DXGI_FORMAT_R32_UINT:
    case DXGI_FORMAT_R32_SINT:
    case DXGI_FORMAT_R24G8_TYPELESS:
    case DXGI_FORMAT_D24_UNORM_S8_UINT:
    case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
    case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
    case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
    case DXGI_FORMAT_R8G8_B8G8_UNORM:
    case DXGI_FORMAT_G8R8_G8B8_UNORM:
    case DXGI_FORMAT_B8G8R8A8_UNORM:
    case DXGI_FORMAT_B8G8R8X8_UNORM:
    case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
    case DXGI_FORMAT_B8G8R8A8_TYPELESS:
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
    case DXGI_FORMAT_B8G8R8X8_TYPELESS:
    case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
    case DXGI_FORMAT_AYUV:
    case DXGI_FORMAT_Y410:
    case DXGI_FORMAT_YUY2: return 32;

    case DXGI_FORMAT_P010:
    case DXGI_FORMAT_P016: return 24;

    case DXGI_FORMAT_R8G8_TYPELESS:
    case DXGI_FORMAT_R8G8_UNORM:
    case DXGI_FORMAT_R8G8_UINT:
    case DXGI_FORMAT_R8G8_SNORM:
    case DXGI_FORMAT_R8G8_SINT:
    case DXGI_FORMAT_R16_TYPELESS:
    case DXGI_FORMAT_R16_FLOAT:
    case DXGI_FORMAT_D16_UNORM:
    case DXGI_FORMAT_R16_UNORM:
    case DXGI_FORMAT_R16_UINT:
    case DXGI_FORMAT_R16_SNORM:
    case DXGI_FORMAT_R16_SINT:
    case DXGI_FORMAT_B5G6R5_UNORM:
    case DXGI_FORMAT_B5G5R5A1_UNORM:
    case DXGI_FORMAT_A8P8:
    case DXGI_FORMAT_B4G4R4A4_UNORM: return 16;

    case DXGI_FORMAT_NV12:
    case DXGI_FORMAT_420_OPAQUE:
    case DXGI_FORMAT_NV11: return 12;

    case DXGI_FORMAT_R8_TYPELESS:
    case DXGI_FORMAT_R8_UNORM:
    case DXGI_FORMAT_R8_UINT:
    case DXGI_FORMAT_R8_SNORM:
    case DXGI_FORMAT_R8_SINT:
    case DXGI_FORMAT_A8_UNORM:
    case DXGI_FORMAT_AI44:
    case DXGI_FORMAT_IA44:
    case DXGI_FORMAT_P8: return 8;

    case DXGI_FORMAT_R1_UNORM: return 1;

    case DXGI_FORMAT_BC1_TYPELESS:
    case DXGI_FORMAT_BC1_UNORM:
    case DXGI_FORMAT_BC1_UNORM_SRGB:
    case DXGI_FORMAT_BC4_TYPELESS:
    case DXGI_FORMAT_BC4_UNORM:
    case DXGI_FORMAT_BC4_SNORM: return 4;

    case DXGI_FORMAT_BC2_TYPELESS:
    case DXGI_FORMAT_BC2_UNORM:
    case DXGI_FORMAT_BC2_UNORM_SRGB:
    case DXGI_FORMAT_BC3_TYPELESS:
    case DXGI_FORMAT_BC3_UNORM:
    case DXGI_FORMAT_BC3_UNORM_SRGB:
    case DXGI_FORMAT_BC5_TYPELESS:
    case DXGI_FORMAT_BC5_UNORM:
    case DXGI_FORMAT_BC5_SNORM:
    case DXGI_FORMAT_BC6H_TYPELESS:
    case DXGI_FORMAT_BC6H_UF16:
    case DXGI_FORMAT_BC6H_SF16:
    case DXGI_FORMAT_BC7_TYPELESS:
    case DXGI_FORMAT_BC7_UNORM:
    case DXGI_FORMAT_BC7_UNORM_SRGB:
    case DXGI_FORMAT_UNKNOWN: return 8;

    default: return 0;
    }
}



D3D12_CLEAR_VALUE* CreateClearValue(const DXGI_FORMAT fmt, uint32_t resourceBindings)
{
    static D3D12_CLEAR_VALUE result;
    ZEROS(result);

    bool isDS = (resourceBindings & to_underlying(EBindFlags::BIND_DEPTH_STENCIL));
    bool isRT = (resourceBindings & to_underlying(EBindFlags::BIND_RENDER_TARGET));
    if (!(isDS||isRT))
        return nullptr;
    switch (static_cast<int>(fmt))
    {
    case DXGI_FORMAT_R32G32B32A32_TYPELESS:
    case DXGI_FORMAT_R32G32B32A32_FLOAT:
    case DXGI_FORMAT_R32G32B32A32_UINT:
    case DXGI_FORMAT_R32G32B32A32_SINT:
    {
        result.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        result.Color[0] = 0;
        result.Color[1] = 0;
        result.Color[2] = 0;
        result.Color[3] = 0;
        return &result;
    }
    case DXGI_FORMAT_R32G32B32_TYPELESS:
    case DXGI_FORMAT_R32G32B32_FLOAT:
    case DXGI_FORMAT_R32G32B32_UINT:
    case DXGI_FORMAT_R32G32B32_SINT:
    {
        result.Format = DXGI_FORMAT_R32G32B32_FLOAT;
        result.Color[0] = 0;
        result.Color[1] = 0;
        result.Color[2] = 0;
        result.Color[3] = 0;
        return &result;
    }

    case DXGI_FORMAT_R16G16B16A16_TYPELESS:
    case DXGI_FORMAT_R16G16B16A16_FLOAT:
    case DXGI_FORMAT_R16G16B16A16_UNORM:
    case DXGI_FORMAT_R16G16B16A16_UINT:
    case DXGI_FORMAT_R16G16B16A16_SNORM:
    case DXGI_FORMAT_R16G16B16A16_SINT:
    {
        result.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
        result.Color[0] = 0;
        result.Color[1] = 0;
        result.Color[2] = 0;
        result.Color[3] = 0;
        return &result;
    }
    case DXGI_FORMAT_R32G32_TYPELESS:
    case DXGI_FORMAT_R32G32_FLOAT:
    case DXGI_FORMAT_R32G32_UINT:
    case DXGI_FORMAT_R32G32_SINT:
    case DXGI_FORMAT_R32G8X24_TYPELESS:
    {
        result.Format = DXGI_FORMAT_R32G32_FLOAT;
        result.Color[0] = 0;
        result.Color[1] = 0;
        result.Color[2] = 0;
        result.Color[3] = 0;
        return &result;
    }
    case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
    {
        result.Format = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
        result.Color[0] = 0;
        result.Color[1] = 0;
        result.Color[2] = 0;
        result.Color[3] = 0;
        return &result;
    }
    case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
    {
        result.Format = DXGI_FORMAT_X32_TYPELESS_G8X24_UINT;
        result.Color[0] = 0;
        result.Color[1] = 0;
        result.Color[2] = 0;
        result.Color[3] = 0;
        return &result;
    }
    case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
    {
        result.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
        result.DepthStencil.Depth = 0;
        result.DepthStencil.Stencil = 0;

        return &result;
    }

    case DXGI_FORMAT_R10G10B10A2_TYPELESS:
    case DXGI_FORMAT_R10G10B10A2_UNORM:
    case DXGI_FORMAT_R10G10B10A2_UINT:
    {
        result.Format = DXGI_FORMAT_R10G10B10A2_UNORM;
        result.Color[0] = 0;
        result.Color[1] = 0;
        result.Color[2] = 0;
        result.Color[3] = 0;
        return &result;
    }
    case DXGI_FORMAT_R8G8B8A8_TYPELESS:
    case DXGI_FORMAT_R8G8B8A8_UNORM:
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
    case DXGI_FORMAT_R8G8B8A8_UINT:
    case DXGI_FORMAT_R8G8B8A8_SNORM:
    case DXGI_FORMAT_R8G8B8A8_SINT:
    {
        result.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        result.Color[0] = 0;
        result.Color[1] = 0;
        result.Color[2] = 0;
        result.Color[3] = 0;
        return &result;
    }
    case DXGI_FORMAT_R11G11B10_FLOAT:
    {
        result.Format = DXGI_FORMAT_R11G11B10_FLOAT;
        result.Color[0] = 0;
        result.Color[1] = 0;
        result.Color[2] = 0;
        result.Color[3] = 0;
        return &result;
    }

    case DXGI_FORMAT_R16G16_TYPELESS:
    case DXGI_FORMAT_R16G16_FLOAT:
    case DXGI_FORMAT_R16G16_UNORM:
    case DXGI_FORMAT_R16G16_UINT:
    case DXGI_FORMAT_R16G16_SNORM:
    case DXGI_FORMAT_R16G16_SINT:
    {
        result.Format = DXGI_FORMAT_R16G16_FLOAT;
        result.Color[0] = 0;
        result.Color[1] = 0;
        result.Color[2] = 0;
        result.Color[3] = 0;
        return &result;
    }

    case DXGI_FORMAT_R32_TYPELESS:
    case DXGI_FORMAT_R32_FLOAT:
    case DXGI_FORMAT_R32_UINT:
    case DXGI_FORMAT_R32_SINT:
    {
        if (isDS)
        {
            result.Format = DXGI_FORMAT_D32_FLOAT;
            result.DepthStencil.Depth = 1;
            result.DepthStencil.Stencil = 0;
            return &result;
        }
        else
        {
            result.Format = DXGI_FORMAT_R32_FLOAT;
            result.Color[0] = 0;
            result.Color[1] = 0;
            result.Color[2] = 0;
            result.Color[3] = 0;
            return &result;
        }
    }

    case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
    {
        if (isDS)
        {
            result.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
            result.DepthStencil.Depth = 1;
            result.DepthStencil.Stencil = 0;
            return &result;
        }
        else
        {
            result.Format = DXGI_FORMAT_X24_TYPELESS_G8_UINT;
            result.Color[0] = 0;
            result.Color[1] = 0;
            result.Color[2] = 0;
            result.Color[3] = 0;
            return &result;
        }
    }
    case DXGI_FORMAT_D32_FLOAT:
    {
        result.Format = DXGI_FORMAT_D32_FLOAT;
        result.DepthStencil.Depth = 1;
        result.DepthStencil.Stencil = 0;

        return &result;
    case DXGI_FORMAT_D24_UNORM_S8_UINT:
    {
        result.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        result.DepthStencil.Depth = 1;
        result.DepthStencil.Stencil = 0;

        return &result;
    }

    case DXGI_FORMAT_R8G8_TYPELESS:
    case DXGI_FORMAT_R8G8_UNORM:
    case DXGI_FORMAT_R8G8_UINT:
    case DXGI_FORMAT_R8G8_SNORM:
    case DXGI_FORMAT_R8G8_SINT:
    case DXGI_FORMAT_R16_TYPELESS:
    case DXGI_FORMAT_R16_FLOAT:
    case DXGI_FORMAT_R16_UNORM:
    case DXGI_FORMAT_R16_UINT:
    case DXGI_FORMAT_R16_SNORM:
    case DXGI_FORMAT_R16_SINT:
    case DXGI_FORMAT_B5G6R5_UNORM:
    case DXGI_FORMAT_B5G5R5A1_UNORM:
    case DXGI_FORMAT_A8P8:
    case DXGI_FORMAT_B4G4R4A4_UNORM:
    {
        result.Format = DXGI_FORMAT_R16_FLOAT;
        result.Color[0] = 0;
        result.Color[1] = 0;
        result.Color[2] = 0;
        result.Color[3] = 0;
        return &result;
    }
    case DXGI_FORMAT_D16_UNORM:
    {
        result.Format = DXGI_FORMAT_D16_UNORM;
        result.DepthStencil.Depth = 0;
        result.DepthStencil.Stencil = 0;

        return &result;
    }

    case DXGI_FORMAT_R8_TYPELESS:
    case DXGI_FORMAT_R8_UNORM:
    case DXGI_FORMAT_R8_UINT:
    case DXGI_FORMAT_R8_SNORM:
    case DXGI_FORMAT_R8_SINT:
    case DXGI_FORMAT_A8_UNORM:
    case DXGI_FORMAT_AI44:
    case DXGI_FORMAT_IA44:
    case DXGI_FORMAT_P8:
    {
        result.Format = DXGI_FORMAT_R8_UNORM;
        result.Color[0] = 0;
        result.Color[1] = 0;
        result.Color[2] = 0;
        result.Color[3] = 0;
        return &result;
    }


    default:
        return nullptr;
    }
    }
}


constexpr D3D12_RESOURCE_STATES inline ToDx12(GpuResource::ResourceState state)
{
    switch (state)
    {
    case GpuResource::ResourceState::RESOURCE_STATE_COMMON: return D3D12_RESOURCE_STATE_COMMON;
    case GpuResource::ResourceState::RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER: return D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
    case GpuResource::ResourceState::RESOURCE_STATE_INDEX_BUFFER: return D3D12_RESOURCE_STATE_INDEX_BUFFER;
    case GpuResource::ResourceState::RESOURCE_STATE_RENDER_TARGET: return D3D12_RESOURCE_STATE_RENDER_TARGET;
    case GpuResource::ResourceState::RESOURCE_STATE_UNORDERED_ACCESS: return D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
    case GpuResource::ResourceState::RESOURCE_STATE_DEPTH_WRITE: return D3D12_RESOURCE_STATE_DEPTH_WRITE;
    case GpuResource::ResourceState::RESOURCE_STATE_DEPTH_READ: return D3D12_RESOURCE_STATE_DEPTH_READ;
    case GpuResource::ResourceState::RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE: return D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
    case GpuResource::ResourceState::RESOURCE_STATE_PIXEL_SHADER_RESOURCE: return D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
    case GpuResource::ResourceState::RESOURCE_STATE_STREAM_OUT: return D3D12_RESOURCE_STATE_STREAM_OUT;
    case GpuResource::ResourceState::RESOURCE_STATE_INDIRECT_ARGUMENT: return D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT;
    case GpuResource::ResourceState::RESOURCE_STATE_COPY_DEST: return D3D12_RESOURCE_STATE_COPY_DEST;
    case GpuResource::ResourceState::RESOURCE_STATE_COPY_SOURCE: return D3D12_RESOURCE_STATE_COPY_SOURCE;
    case GpuResource::ResourceState::RESOURCE_STATE_RESOLVE_DEST: return D3D12_RESOURCE_STATE_RESOLVE_DEST;
    case GpuResource::ResourceState::RESOURCE_STATE_RESOLVE_SOURCE: return D3D12_RESOURCE_STATE_RESOLVE_SOURCE;
    case GpuResource::ResourceState::RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE: return D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;
    case GpuResource::ResourceState::RESOURCE_STATE_SHADING_RATE_SOURCE: return D3D12_RESOURCE_STATE_SHADING_RATE_SOURCE;
    case GpuResource::ResourceState::RESOURCE_STATE_GENERIC_READ: return D3D12_RESOURCE_STATE_GENERIC_READ;
    case GpuResource::ResourceState::RESOURCE_STATE_PRESENT: return D3D12_RESOURCE_STATE_PRESENT;
    case GpuResource::ResourceState::RESOURCE_STATE_UNDEFINED:
        assert(false);
        return D3D12_RESOURCE_STATE_VIDEO_DECODE_READ; //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    }
}


D3D12_FILTER inline ToD3D11SamplerFilter[] = {
    D3D12_FILTER_MIN_MAG_MIP_POINT,
    D3D12_FILTER_MIN_MAG_MIP_POINT,
    D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR,
    D3D12_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT,
    D3D12_FILTER_MIN_POINT_MAG_MIP_LINEAR,
    D3D12_FILTER_MIN_LINEAR_MAG_MIP_POINT,
    D3D12_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR,
    D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT,
    D3D12_FILTER_MIN_MAG_MIP_LINEAR,
    D3D12_FILTER_ANISOTROPIC,
    D3D12_FILTER_COMPARISON_MIN_MAG_MIP_POINT,
    D3D12_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR,
    D3D12_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT,
    D3D12_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR,
    D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT,
    D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR,
    D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT,
    D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR,
    D3D12_FILTER_COMPARISON_ANISOTROPIC,
    D3D12_FILTER_MINIMUM_MIN_MAG_MIP_POINT,
    D3D12_FILTER_MINIMUM_MIN_MAG_POINT_MIP_LINEAR,
    D3D12_FILTER_MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT,
    D3D12_FILTER_MINIMUM_MIN_POINT_MAG_MIP_LINEAR,
    D3D12_FILTER_MINIMUM_MIN_LINEAR_MAG_MIP_POINT,
    D3D12_FILTER_MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR,
    D3D12_FILTER_MINIMUM_MIN_MAG_LINEAR_MIP_POINT,
    D3D12_FILTER_MINIMUM_MIN_MAG_MIP_LINEAR,
    D3D12_FILTER_MINIMUM_ANISOTROPIC,
    D3D12_FILTER_MAXIMUM_MIN_MAG_MIP_POINT,
    D3D12_FILTER_MAXIMUM_MIN_MAG_POINT_MIP_LINEAR,
    D3D12_FILTER_MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT,
    D3D12_FILTER_MAXIMUM_MIN_POINT_MAG_MIP_LINEAR,
    D3D12_FILTER_MAXIMUM_MIN_LINEAR_MAG_MIP_POINT,
    D3D12_FILTER_MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR,
    D3D12_FILTER_MAXIMUM_MIN_MAG_LINEAR_MIP_POINT,
    D3D12_FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR,
    D3D12_FILTER_MAXIMUM_ANISOTROPIC
};

D3D12_SAMPLER_DESC inline ToD3D12(const Compressed::SamplerStateDesc& state)
{
    D3D12_SAMPLER_DESC desc{};
    ZEROS(desc);
    assert(state.Fields.Filter != 0);
    desc.Filter = ToD3D11SamplerFilter[state.Fields.Filter];
    desc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    desc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    desc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    desc.MaxAnisotropy = state.Fields.MaxAnisotropy;
    desc.MaxLOD = state.MaxLOD;
    desc.MinLOD = state.MinLOD;
    desc.MipLODBias = 0;
    return desc;
}

/*
BIND_VERTEX_BUFFER          = 0x1L,
BIND_INDEX_BUFFER           = 0x2L,
BIND_CONSTANT_BUFFER        = 0x4L,
BIND_SHADER_RESOURCE        = 0x8L,
BIND_STREAM_OUTPUT          = 0x10L,
BIND_RENDER_TARGET          = 0x20L,
BIND_DEPTH_STENCIL          = 0x40L,
BIND_UNORDERED_ACCESS       = 0x80L,
*/
/*
D3D12_RESOURCE_FLAG_NONE	= 0,
D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET	= 0x1,
D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL	= 0x2,
D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS	= 0x4,
D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE	= 0x8,
D3D12_RESOURCE_FLAG_ALLOW_CROSS_ADAPTER	= 0x10,
D3D12_RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS	= 0x20,
D3D12_RESOURCE_FLAG_VIDEO_DECODE_REFERENCE_ONLY	= 0x40
*/

D3D12_RESOURCE_FLAGS ToDx12TextureFlags(uint32_t bindings)
{
    uint32_t result = 0;

    if (bindings & to_underlying(EBindFlags::BIND_RENDER_TARGET))
        result |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
    if (bindings & to_underlying(EBindFlags::BIND_DEPTH_STENCIL))
        result |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
    if (bindings & to_underlying(EBindFlags::BIND_UNORDERED_ACCESS))
        result |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

    return D3D12_RESOURCE_FLAGS(result);
}


D3D12_PRIMITIVE_TOPOLOGY ToD3DPT[] = {
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

D3D12_PRIMITIVE_TOPOLOGY_TYPE ToD3DPT_Type[] = {
    D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED,
    D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT,
    D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE,
    D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE,
    D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED,
    D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
    D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
    D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE,
    D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE,
    D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
    D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
    D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH,
    D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH,
    D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH,
    D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH,
};


D3D12_DEPTH_STENCIL_DESC ToDX12DSState(const Compressed::DepthStencilStateDesc& depthStencilState)
{
    const std::array<D3D12_STENCIL_OP, 9> depthOpArray = {
        D3D12_STENCIL_OP(0),
        D3D12_STENCIL_OP_KEEP,
        D3D12_STENCIL_OP_ZERO,
        D3D12_STENCIL_OP_REPLACE,
        D3D12_STENCIL_OP_INCR_SAT,
        D3D12_STENCIL_OP_DECR_SAT,
        D3D12_STENCIL_OP_INVERT,
        D3D12_STENCIL_OP_INCR,
        D3D12_STENCIL_OP_DECR
    };

    const std::array<D3D12_COMPARISON_FUNC, 9> depthCompArray = {
        D3D12_COMPARISON_FUNC(0),
        D3D12_COMPARISON_FUNC_NEVER,
        D3D12_COMPARISON_FUNC_LESS,
        D3D12_COMPARISON_FUNC_EQUAL,
        D3D12_COMPARISON_FUNC_LESS_EQUAL,
        D3D12_COMPARISON_FUNC_GREATER,
        D3D12_COMPARISON_FUNC_NOT_EQUAL,
        D3D12_COMPARISON_FUNC_GREATER_EQUAL,
        D3D12_COMPARISON_FUNC_ALWAYS
    };


    D3D12_DEPTH_STENCIL_DESC desc{};
    ZEROS(desc);
    desc.BackFace.StencilFailOp = depthOpArray[depthStencilState.Fields.BackStencilFailOp];
    desc.BackFace.StencilFunc = depthCompArray[depthStencilState.Fields.BackStencilFunc];
    desc.BackFace.StencilPassOp = depthOpArray[depthStencilState.Fields.BackStencilPassOp];
    desc.BackFace.StencilDepthFailOp = depthOpArray[depthStencilState.Fields.BackStencilDepthFailOp];

    desc.BackFace.StencilFailOp = depthOpArray[depthStencilState.Fields.FrontStencilFailOp];
    desc.BackFace.StencilFunc = depthCompArray[depthStencilState.Fields.FrontStencilFunc];
    desc.BackFace.StencilPassOp = depthOpArray[depthStencilState.Fields.FrontStencilPassOp];
    desc.BackFace.StencilDepthFailOp = depthOpArray[depthStencilState.Fields.FrontStencilDepthFailOp];

    desc.DepthEnable = depthStencilState.Fields.DepthEnable;
    desc.DepthFunc = depthCompArray[depthStencilState.Fields.DepthFunc];
    desc.StencilEnable = depthCompArray[depthStencilState.Fields.StencilEnable];
    desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK(depthStencilState.Fields.DepthWriteMask);
    desc.StencilWriteMask = depthStencilState.Fields.StencilWriteMask;
    desc.StencilReadMask = depthStencilState.Fields.StencilReadMask;

    return desc;
}


D3D12_RASTERIZER_DESC ToDX12RSState(const Compressed::RasterizerStateDesc& rasterizerState)
{
    D3D12_RASTERIZER_DESC result{};
    ZEROS(result);
    result.CullMode =
        std::array<D3D12_CULL_MODE, 4>({
            D3D12_CULL_MODE_NONE,
            D3D12_CULL_MODE_NONE,
            D3D12_CULL_MODE_FRONT,
            D3D12_CULL_MODE_BACK
        })
        [rasterizerState.Fields.CullMode];
    result.FillMode =
        std::array<D3D12_FILL_MODE, 3>({
            D3D12_FILL_MODE(0),
            D3D12_FILL_MODE_WIREFRAME,
            D3D12_FILL_MODE_SOLID
        })
        [rasterizerState.Fields.FillMode];
    result.FrontCounterClockwise = rasterizerState.Fields.FrontCounterClockwise;
    result.DepthClipEnable = rasterizerState.Fields.DepthClipEnable;
    //result.ScissorEnable = rasterizerState.Fields.ScissorEnable;
    result.DepthBiasClamp = D3D12_FLOAT32_MAX;
    result.MultisampleEnable = false;

    return result;
}
