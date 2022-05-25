#include "RenderDeviceDX11.h"

using namespace GVM;

RenderDeviceDX11::RenderDeviceDX11(const RenderDeviceInitParams& initParams, bool debugMode)
:IRenderDevice(initParams, debugMode)
{
}

RenderDeviceDX11::~RenderDeviceDX11()
{
}

const IStructuresSize RenderDeviceDX11::GetClassesSize()
{
    return{
        sizeof(ResourceDX11),
        sizeof(ResourceViewDX11),
        sizeof(ShaderDX11),
        sizeof(InputLayoutDX11)
        };
}

const IRenderDevice::IResource* RenderDeviceDX11::CreateResource(const ResourceDesc& desc)
{
    return &resource;
}

const IRenderDevice::IConstBufferView* RenderDeviceDX11::CreateConstBufferView(const ConstBufferViewDesc& desc)
{
    return reinterpret_cast<IConstBufferView*>(&resourceView);
}

const IRenderDevice::IVertexBufferView* RenderDeviceDX11::CreateVertexBufferView(const VertexBufferViewDesc& desc)
{
    return reinterpret_cast<IVertexBufferView*>(&resourceView);
}

const IRenderDevice::IIndexBufferView* RenderDeviceDX11::CreateIndexBufferView(const IndexBufferViewDesc& desc)
{
    return reinterpret_cast<IIndexBufferView*>(&resourceView);
}

const IRenderDevice::IDepthStencilView* RenderDeviceDX11::CreateDepthStencilView(const DepthStencilViewDesc& desc)
{
    return reinterpret_cast<IDepthStencilView*>(&resourceView);
}

const IRenderDevice::IShaderResourceView* RenderDeviceDX11::CreateShaderResourceView(const ShaderResourceViewDesc& desc)
{
    return reinterpret_cast<IShaderResourceView*>(&resourceView);
}

const IRenderDevice::IRenderTargetView* RenderDeviceDX11::CreateRenderTargetView(const RenderTargetViewDesc& desc)
{
    return reinterpret_cast<IRenderTargetView*>(&resourceView);
}

const IRenderDevice::IUATargetView* RenderDeviceDX11::CreateUATargetView(const UATargetViewDesc& desc)
{
    return reinterpret_cast<IUATargetView*>(&resourceView);
}
