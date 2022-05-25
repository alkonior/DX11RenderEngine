#pragma once
#include "VirtualMachine/IRenderDevice.h"
#include "BaseDX11Classes.h"

namespace GVM
{
    
class RenderDeviceDX11 : public IRenderDevice
{
public:
    ResourceDX11 resource;
    ResourceViewDX11 resourceView;
    ShaderDX11 shader;
    InputLayoutDX11 inputLayout;
    

    RenderDeviceDX11(const RenderDeviceInitParams& initParams, bool debugMode);
    RenderDeviceDX11(const IRenderDevice& render_device) = delete;
    RenderDeviceDX11(const IRenderDevice&& render_device) = delete;
    ~RenderDeviceDX11() override;
    
protected:
    const IStructuresSize GetClassesSize() override;
    
    const IResource* CreateResource(const ResourceDesc& desc) override;
    const IConstBufferView* CreateConstBufferView(const ConstBufferViewDesc& desc) override;
    const IVertexBufferView* CreateVertexBufferView(const VertexBufferViewDesc& desc) override;
    const IIndexBufferView* CreateIndexBufferView(const IndexBufferViewDesc& desc) override;
    const IDepthStencilView* CreateDepthStencilView(const DepthStencilViewDesc& desc) override;
    const IShaderResourceView* CreateShaderResourceView(const ShaderResourceViewDesc& desc) override;
    const IRenderTargetView* CreateRenderTargetView(const RenderTargetViewDesc& desc) override;
    const IUATargetView* CreateUATargetView(const UATargetViewDesc& desc) override;
};
    
}
