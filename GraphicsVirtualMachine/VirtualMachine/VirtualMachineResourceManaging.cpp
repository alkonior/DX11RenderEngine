#include "VirtualMachine.h"


const GVM::Resource* GVM::VirtualMachine::CreateResource(const ResourceDesc& desc)
{
    auto& resource = resourcesManager.CreateResource(desc);

    return resource.id;
}

const GVM::Resource* GVM::VirtualMachine::CreateBuffer(const BufferResourceDesc& desc)
{
    auto& resource = resourcesManager.CreateResource(desc);

    return resource.id;
}

const GVM::Resource* GVM::VirtualMachine::CreateTexture1D(const Texture1DResourceDesc& desc)
{
    auto& resource = resourcesManager.CreateResource(desc);

    return resource.id;
}

const GVM::Resource* GVM::VirtualMachine::CreateTexture2D(const Texture2DResourceDesc& desc)
{
    auto& resource = resourcesManager.CreateResource(desc);

    return resource.id;
}

const GVM::Resource* GVM::VirtualMachine::CreateTexture3D(const Texture3DResourceDesc& desc)
{
    auto& resource = resourcesManager.CreateResource(desc);

    return resource.id;
}

const GVM::Resource* GVM::VirtualMachine::CreateTextureCube(const TextureCubeResourceDesc& desc)
{
    auto& resource = resourcesManager.CreateResource(desc);

    return resource.id;
}

const GVM::VertexBuffer* GVM::VirtualMachine::CreateVertexBuffer(const BufferResourceDesc& desc)
{
    auto& resource = resourcesManager.CreateResource(desc);

    return resource.id;
}

const GVM::ConstBuffer* GVM::VirtualMachine::CreateConstBuffer(const BufferResourceDesc& desc)
{
    auto& resource = resourcesManager.CreateResource(desc);

    return resource.id;
}

const GVM::IndexBuffer* GVM::VirtualMachine::CreateIndexBuffer(const BufferResourceDesc& desc)
{
    auto& resource = resourcesManager.CreateResource(desc);

    return resource.id;
}

