#include "VirtualMachine.h"

using namespace GVM;



Resource* VirtualMachine::CreateResource(const ResourceDesc& desc)
{
    auto& resource = resourcesManager.CreateResource(desc);
    //resource.resource = RenderDevice->CreateResource(resource.resourceDescription);
    //todo Command
    if(resource.resource == nullptr)
        PushCommand(EMachineCommands::CREATE_RESOURCE);
    else
        PushCommand(EMachineCommands::UPDATE_RESOURCE);
        
    PushData(&resource, sizeof(GpuResource));
    
    return resource.id;
}

Resource* VirtualMachine::CreateBuffer(const BufferResourceDesc& desc)
{
    auto& resource = resourcesManager.CreateBuffer(desc);
    //todo Command
    //resource.resource = RenderDevice->CreateResource(resource.resourceDescription);
    
    if(resource.resource == nullptr)
        PushCommand(EMachineCommands::CREATE_RESOURCE);
    else
        PushCommand(EMachineCommands::UPDATE_RESOURCE);
    
    PushData(&resource, sizeof(GpuResource));
    
    return resource.id;
}

Resource* VirtualMachine::CreateTexture1D(const Texture1DResourceDesc& desc)
{
    auto& resource = resourcesManager.CreateTexture1D(desc);
    //resource.resource = RenderDevice->CreateResource(resource.resourceDescription);
    //todo Command
    
    if(resource.resource == nullptr)
        PushCommand(EMachineCommands::CREATE_RESOURCE);
    else
        PushCommand(EMachineCommands::UPDATE_RESOURCE);
    
    PushData(&resource, sizeof(GpuResource));
    
    return resource.id;
}

Resource* VirtualMachine::CreateTexture2D(const Texture2DResourceDesc& desc)
{
    auto& resource = resourcesManager.CreateTexture2D(desc);
    //resource.resource = RenderDevice->CreateResource(resource.resourceDescription);
    //todo Command
    
    if(resource.resource == nullptr)
        PushCommand(EMachineCommands::CREATE_RESOURCE);
    else
        PushCommand(EMachineCommands::UPDATE_RESOURCE);
    
    PushData(&resource, sizeof(GpuResource));
    
    return resource.id;
}

Resource* VirtualMachine::CreateTexture3D(const Texture3DResourceDesc& desc)
{
    auto& resource = resourcesManager.CreateTexture3D(desc);
    //resource.resource = RenderDevice->CreateResource(resource.resourceDescription);
    //todo Command
    
    
    if(resource.resource == nullptr)
        PushCommand(EMachineCommands::CREATE_RESOURCE);
    else
        PushCommand(EMachineCommands::UPDATE_RESOURCE);
    
    PushData(&resource, sizeof(GpuResource));
    
    return resource.id;
}

Resource* VirtualMachine::CreateTextureCube(const TextureCubeResourceDesc& desc)
{
    auto& resource = resourcesManager.CreateTextureCube(desc);
    //resource.resource = RenderDevice->CreateResource(resource.resourceDescription);
    //todo Command
    
    
    if(resource.resource == nullptr)
        PushCommand(EMachineCommands::CREATE_RESOURCE);
    else
        PushCommand(EMachineCommands::UPDATE_RESOURCE);
    
    PushData(&resource, sizeof(GpuResource));
    
    return resource.id;
}

VertexBuffer* VirtualMachine::CreateVertexBuffer(const BufferResourceDesc& desc)
{
    auto& resource = resourcesManager.CreateVertexBuffer(desc);
    //resource.resource = RenderDevice->CreateResource(resource.resourceDescription);
    //todo Command
    
    
    if(resource.resource == nullptr)
        PushCommand(EMachineCommands::CREATE_RESOURCE);
    else
        PushCommand(EMachineCommands::UPDATE_RESOURCE);
    
    PushData(&resource, sizeof(GpuResource));
    
    return reinterpret_cast<VertexBuffer*>(resource.id);
}

ConstBuffer* VirtualMachine::CreateConstBuffer(const BufferResourceDesc& desc)
{
    auto& resource = resourcesManager.CreateConstBuffer(desc);
    //resource.resource = RenderDevice->CreateResource(resource.resourceDescription);
    //todo Command
    
    if(resource.resource == nullptr)
        PushCommand(EMachineCommands::CREATE_RESOURCE);
    else
        PushCommand(EMachineCommands::UPDATE_RESOURCE);
    
    PushData(&resource, sizeof(GpuResource));
    
    return reinterpret_cast<ConstBuffer*>(resource.id);
}

IndexBuffer* VirtualMachine::CreateIndexBuffer(const BufferResourceDesc& desc)
{
    auto& resource = resourcesManager.CreateIndexBuffer(desc);
    //resource.resource = RenderDevice->CreateResource(resource.resourceDescription);
    //todo Command
   
    if(resource.resource == nullptr)
        PushCommand(EMachineCommands::CREATE_RESOURCE);
    else
        PushCommand(EMachineCommands::UPDATE_RESOURCE);
    
    PushData(&resource, sizeof(GpuResource));
    
    return reinterpret_cast<IndexBuffer*>(resource.id);
}

ConstBufferView* VirtualMachine::CreateConstBufferView(const ConstBufferViewDesc& desc)
{
    auto& resourceView = resourcesManager.CreateConstBufferView(desc);
    //resourceView.view = RenderDevice->CreateConstBufferView(resourceView.cbViewDescription);
    //todo Command
    return reinterpret_cast<ConstBufferView*>(resourceView.id);
}

VertexBufferView* VirtualMachine::CreateVertexBufferView(const VertexBufferViewDesc& desc)
{
    auto& resourceView = resourcesManager.CreateVertexBufferView(desc);
    resourceView.view = RenderDevice->CreateConstBufferView(resourceView.cbViewDescription);
    //todo Command
    return reinterpret_cast<VertexBufferView*>(resourceView.id);
}

IndexBufferView* VirtualMachine::CreateIndexBufferView(const IndexBufferViewDesc& desc)
{
    auto& resourceView = resourcesManager.CreateIndexBufferView(desc);
    resourceView.view = RenderDevice->CreateIndexBufferView(resourceView.ibViewDescription);
    //todo Command
    return reinterpret_cast<IndexBufferView*>(resourceView.id);
}

DepthStencilView* VirtualMachine::CreateDepthStencilView(const DepthStencilViewDesc& desc)
{
    auto& resourceView = resourcesManager.CreateDepthStencilView(desc);
    resourceView.view = RenderDevice->CreateDepthStencilView(resourceView.dbViewDescription);
    //todo Command
    return reinterpret_cast<DepthStencilView*>(resourceView.id);
}

ShaderResourceView* VirtualMachine::CreateShaderResourceView(const ShaderResourceViewDesc& desc)
{
    auto& resourceView = resourcesManager.CreateShaderResourceView(desc);
    resourceView.view = RenderDevice->CreateShaderResourceView(resourceView.srViewDescription);
    //todo Command
    return reinterpret_cast<ShaderResourceView*>(resourceView.id);
}

RenderTargetView* VirtualMachine::CreateRenderTargetView(const RenderTargetViewDesc& desc)
{
    auto& resourceView = resourcesManager.CreateRenderTargetView(desc);
    resourceView.view = RenderDevice->CreateRenderTargetView(resourceView.rtViewDescription);
    //todo Command
    return reinterpret_cast<RenderTargetView*>(resourceView.id);
}

UATargetView* VirtualMachine::CreateUATargetView(const UATargetViewDesc& desc)
{
    auto& resourceView = resourcesManager.CreateUATargetView(desc);
    resourceView.view = RenderDevice->CreateUATargetView(resourceView.uaViewDescription);
    //todo Command
    return reinterpret_cast<UATargetView*>(resourceView.id);
}

void VirtualMachine::AddDisposeResource(const Resource* resource)
{
    resourcesManager.AddDisposeResource(resource);
}

void VirtualMachine::AddDisposeResourceView(const ResourceView* resourceView)
{
    resourcesManager.AddDisposeResourceView(resourceView);
}

Shader* VirtualMachine::CreateShader(const ShaderDesc& desc)
{
    auto& shader = resourcesManager.CreateShader(desc);
    shader.shader = RenderDevice->CreateShader(shader.description);
    return reinterpret_cast<Shader*>(shader.id);
}

InputLayout* VirtualMachine::CreateInputLayout(const InputAssemblerDeclarationDesc& desc)
{
    auto& inputLayout = resourcesManager.CreateInputLayout(desc);
    inputLayout.inputLayout = RenderDevice->CreateInputLayout(inputLayout.description);
    return reinterpret_cast<InputLayout*>(inputLayout.id);
}

void VirtualMachine::SetResourceData(Resource* resource, uint16_t dstSubresource, const UBox& rect,
    const void* pSrcData, int32_t srcRowPitch, int32_t srcDepthPitch)
{
    PushCommand(EMachineCommands::SET_RESOURCE_DATA);

    ResourceUpdateData rud = {dstSubresource,srcRowPitch,srcRowPitch};
    PushData(&rud, sizeof(rud));
    
    uint32_t dataSize = (rect.Back-rect.Front)*(rect.Right-rect.Left)*(rect.Bottom-rect.Top);
    PushData(pSrcData, dataSize);
}

void VirtualMachine::SetVertexBufferData(VertexBuffer* vertexBuffer, const void* pSrcData, uint32_t dataLength,
    int32_t srcRowPitch, int32_t srcDepthPitch)
{
    UBox rect {0,0,0,dataLength,1,1};
    SetResourceData(vertexBuffer, 0, rect,pSrcData,srcRowPitch,srcDepthPitch);
}

void VirtualMachine::SetIndexBufferData(IndexBuffer* buffer, const void* pSrcData, uint32_t dataLength,
    int32_t srcRowPitch, int32_t srcDepthPitch)
{
    UBox rect {0,0,0,dataLength,1,1};
    SetResourceData(buffer, 0, rect,pSrcData,srcRowPitch,srcDepthPitch);
}

void VirtualMachine::SetConstBufferData(ConstBuffer* constBuffer, const void* data, uint32_t dataSize)
{
    UBox rect {0,0,0,dataSize,1,1};
    SetResourceData(constBuffer, 0, rect,data,0,0);
}