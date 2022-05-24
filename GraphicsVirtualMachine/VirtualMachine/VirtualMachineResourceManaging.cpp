#include "VirtualMachine.h"

using namespace GVM;



const Resource* VirtualMachine::CreateResource(const ResourceDesc& desc)
{
    auto& resource = resourcesManager.CreateResource(desc);

    return resource.id;
}

const Resource* VirtualMachine::CreateBuffer(const BufferResourceDesc& desc)
{
    auto& resource = resourcesManager.CreateBuffer(desc);

    return resource.id;
}

const Resource* VirtualMachine::CreateTexture1D(const Texture1DResourceDesc& desc)
{
    auto& resource = resourcesManager.CreateTexture1D(desc);

    return resource.id;
}

const Resource* VirtualMachine::CreateTexture2D(const Texture2DResourceDesc& desc)
{
    auto& resource = resourcesManager.CreateTexture2D(desc);

    return resource.id;
}

const Resource* VirtualMachine::CreateTexture3D(const Texture3DResourceDesc& desc)
{
    auto& resource = resourcesManager.CreateTexture3D(desc);

    return resource.id;
}

const Resource* VirtualMachine::CreateTextureCube(const TextureCubeResourceDesc& desc)
{
    auto& resource = resourcesManager.CreateTextureCube(desc);

    return resource.id;
}

const VertexBuffer* VirtualMachine::CreateVertexBuffer(const BufferResourceDesc& desc)
{
    auto& resource = resourcesManager.CreateVertexBuffer(desc);

    return reinterpret_cast<const VertexBuffer*>(resource.id);
}

const ConstBuffer* VirtualMachine::CreateConstBuffer(const BufferResourceDesc& desc)
{
    auto& resource = resourcesManager.CreateConstBuffer(desc);

    return reinterpret_cast<const ConstBuffer*>(resource.id);
}

const IndexBuffer* VirtualMachine::CreateIndexBuffer(const BufferResourceDesc& desc)
{
    auto& resource = resourcesManager.CreateIndexBuffer(desc);

    return reinterpret_cast<const IndexBuffer*>(resource.id);
}

const ConstBufferView* VirtualMachine::CreateConstBufferView(const ConstBufferViewDesc& desc)
{
    auto& resourceView = resourcesManager.CreateConstBufferView(desc);

    return reinterpret_cast<const ConstBufferView*>(resourceView.id);
}

const VertexBufferView* VirtualMachine::CreateVertexBufferView(const VertexBufferViewDesc& desc)
{
    auto& resourceView = resourcesManager.CreateVertexBufferView(desc);

    return reinterpret_cast<const VertexBufferView*>(resourceView.id);
}

const IndexBufferView* VirtualMachine::CreateIndexBufferView(const IndexBufferViewDesc& desc)
{
    auto& resourceView = resourcesManager.CreateIndexBufferView(desc);

    return reinterpret_cast<const IndexBufferView*>(resourceView.id);
}

const DepthStencilView* VirtualMachine::CreateDepthStencilView(const DepthStencilViewDesc& desc)
{
    auto& resourceView = resourcesManager.CreateDepthStencilView(desc);

    return reinterpret_cast<const DepthStencilView*>(resourceView.id);
}

const ShaderResourceView* VirtualMachine::CreateShaderResourceView(const ShaderResourceViewDesc& desc)
{
    auto& resourceView = resourcesManager.CreateShaderResourceView(desc);

    return reinterpret_cast<const ShaderResourceView*>(resourceView.id);
}

const RenderTargetView* VirtualMachine::CreateRenderTargetView(const RenderTargetViewDesc& desc)
{
    auto& resourceView = resourcesManager.CreateRenderTargetView(desc);

    return reinterpret_cast<const RenderTargetView*>(resourceView.id);
}

const UATargetView* VirtualMachine::CreateUATargetView(const UATargetViewDesc& desc)
{
    auto& resourceView = resourcesManager.CreateUATargetView(desc);

    return reinterpret_cast<const UATargetView*>(resourceView.id);
}

void VirtualMachine::AddDisposeResource(const Resource* resource)
{
    resourcesManager.AddDisposeResource(resource);
}

void VirtualMachine::AddDisposeResourceView(const ResourceView* resourceView)
{
    resourcesManager.AddDisposeResourceView(resourceView);
}

