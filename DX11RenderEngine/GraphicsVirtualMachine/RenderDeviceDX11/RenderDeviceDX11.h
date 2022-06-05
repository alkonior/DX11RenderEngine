#pragma once
#include "../VirtualMachine/IRenderDevice.h"
#include "BaseDX11Classes.h"

namespace GVM {

class RenderDeviceDX11 : public IRenderDevice {
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
    IStructuresSize GetClassesSize() override;

    IResource* CreateResource(const ResourceDesc& desc) override;
    IConstBufferView* CreateConstBufferView(const ConstBufferViewDesc& desc) override;
    IVertexBufferView* CreateVertexBufferView(const VertexBufferViewDesc& desc) override;
    IIndexBufferView* CreateIndexBufferView(const IndexBufferViewDesc& desc) override;
    IDepthStencilView* CreateDepthStencilView(const DepthStencilViewDesc& desc) override;
    IShaderResourceView* CreateShaderResourceView(const ShaderResourceViewDesc& desc) override;
    IRenderTargetView* CreateRenderTargetView(const RenderTargetViewDesc& desc) override;
    IUATargetView* CreateUATargetView(const UATargetViewDesc& desc) override;

    IShader* CreateShader(const ShaderDesc& desc) override;
    IInputLayout* CreateInputLayout(const InputAssemblerDeclarationDesc& desc) override;
};

}
