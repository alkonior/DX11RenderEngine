#include "winHandler.h"

using namespace GVM;

static UINT CalcConstantBufferByteSize(UINT byteSize) {
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


D3D12_DEPTH_STENCIL_VIEW_DESC ToD3D12DepthStencilView(const DepthStencilViewDesc& desc)
{
    D3D12_DEPTH_STENCIL_VIEW_DESC result;
    result.Format = ToD3D_DepthFormat[to_underlying(desc.Format)];
    result.Flags = (D3D12_DSV_FLAGS)to_underlying(desc.Flag);
    result.Format = ToD3D_DepthFormat[to_underlying(desc.Format)];
    result.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
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


D3D12_SHADER_RESOURCE_VIEW_DESC ToD3D12ShaderView(const ShaderResourceViewDesc& desc)
{
    D3D12_SHADER_RESOURCE_VIEW_DESC result;
    result.Format = ToD3D_TextureFormat[to_underlying(desc.Format)];
    result.ViewDimension = ToD3D_ShaderViewDimension[to_underlying(desc.Dimension)];
    result.Texture2D.MipLevels = 1;
    result.Texture2D.MostDetailedMip = 0;
    return result;
}

D3D12_RENDER_TARGET_VIEW_DESC ToD3D12RTView(const RenderTargetViewDesc& desc)
{
    D3D12_RENDER_TARGET_VIEW_DESC result;
    result.Format = ToD3D_TextureFormat[to_underlying(desc.Format)];
    result.ViewDimension = ToD3D_RTViewDimension[to_underlying(desc.Dimension)];
    result.Texture2D.MipSlice = 0;
    return result;
}

D3D12_UNORDERED_ACCESS_VIEW_DESC ToD3D12UAView(const UATargetViewDesc& desc)
{
    D3D12_UNORDERED_ACCESS_VIEW_DESC result;
    result.Format = ToD3D_TextureFormat[to_underlying(desc.Format)];
    result.ViewDimension = ToD3D_UAViewDimension[to_underlying(desc.Dimension)];
    result.Texture2D.MipSlice = 0;
    return result;
}


std::vector<D3D12_INPUT_ELEMENT_DESC> ToD3D11(const InputAssemblerDeclarationDesc& desc)
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

bool ToD3D11Viewports(const Compressed::ViewportDesc viewports[], D3D12_VIEWPORT d3d11viewports[20], uint8_t num)
{
    bool result = false;
    for (int i = 0; i < num; i++)
    {
        if (((D3D12_VIEWPORT*)viewports)[i] != d3d11viewports[i])
        {
            d3d11viewports[i] = ((D3D12_VIEWPORT*)viewports)[i];
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
D3D12_BLEND_DESC ToD3D12Blend(const Compressed::CoreBlendDesc& blendState)
{
    D3D12_BLEND_DESC desc{D3D12_BLEND_DESC()};
    desc.AlphaToCoverageEnable = 0;
    desc.IndependentBlendEnable = 0;
        
    for (int i =0; i< 8; i++)
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