#include "RenderDeviceDX11.h"

using namespace GVM;

RenderDeviceDX11::RenderDeviceDX11(const RenderDeviceInitParams& initParams, bool debugMode)
    : IRenderDevice(initParams, debugMode)
{}

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

IRenderDevice::IResource* RenderDeviceDX11::CreateResource(const ResourceDesc& desc)
{
    return &resource;
}

IRenderDevice::IConstBufferView* RenderDeviceDX11::CreateConstBufferView(const ConstBufferViewDesc& desc)
{
    return reinterpret_cast<IConstBufferView*>(&resourceView);
}

IRenderDevice::IVertexBufferView* RenderDeviceDX11::CreateVertexBufferView(const VertexBufferViewDesc& desc)
{
    return reinterpret_cast<IVertexBufferView*>(&resourceView);
}

IRenderDevice::IIndexBufferView* RenderDeviceDX11::CreateIndexBufferView(const IndexBufferViewDesc& desc)
{
    return reinterpret_cast<IIndexBufferView*>(&resourceView);
}

IRenderDevice::IDepthStencilView* RenderDeviceDX11::CreateDepthStencilView(const DepthStencilViewDesc& desc)
{
    return reinterpret_cast<IDepthStencilView*>(&resourceView);
}

IRenderDevice::IShaderResourceView* RenderDeviceDX11::CreateShaderResourceView(const ShaderResourceViewDesc& desc)
{
    return reinterpret_cast<IShaderResourceView*>(&resourceView);
}

IRenderDevice::IRenderTargetView* RenderDeviceDX11::CreateRenderTargetView(const RenderTargetViewDesc& desc)
{
    return reinterpret_cast<IRenderTargetView*>(&resourceView);
}

IRenderDevice::IUATargetView* RenderDeviceDX11::CreateUATargetView(const UATargetViewDesc& desc)
{
    return reinterpret_cast<IUATargetView*>(&resourceView);
}

IRenderDevice::IShader* RenderDeviceDX11::CreateShader(const ShaderDesc& desc)
{
    return &shader;
}

IRenderDevice::IInputLayout* RenderDeviceDX11::CreateInputLayout(const InputAssemblerDeclarationDesc& desc)
{
    return &inputLayout;
}
