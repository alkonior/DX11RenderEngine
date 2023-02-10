#include "VirtualMachine.h"

using namespace GVM;



Resource* VirtualMachine::CreateResource(const ResourceDesc& desc)
{
    auto& resource = resourcesManager.CreateResource(desc);
    //resource.resource = RenderDevice->CreateResource(resource.resourceDescription);
    //todo Command

    PushData((void*)resource.id);

    if (resource.resource == nullptr)
        PushCommand(EMachineCommands::CREATE_RESOURCE);
    else
        PushCommand(EMachineCommands::UPDATE_RESOURCE);


    return resource.id;
}

Resource* VirtualMachine::CreateBuffer(const BufferResourceDesc& desc)
{
    auto& resource = resourcesManager.CreateBuffer(desc);
    //todo Command
    //resource.resource = RenderDevice->CreateResource(resource.resourceDescription);

    PushData((void*)resource.id);

    if (resource.resource == nullptr)
        PushCommand(EMachineCommands::CREATE_RESOURCE);
    else
        PushCommand(EMachineCommands::UPDATE_RESOURCE);

    return resource.id;
}

Resource* VirtualMachine::CreateTexture1D(const Texture1DResourceDesc& desc)
{
    auto& resource = resourcesManager.CreateTexture1D(desc);
    //resource.resource = RenderDevice->CreateResource(resource.resourceDescription);
    //todo Command

    PushData((void*)resource.id);

    if (resource.resource == nullptr)
        PushCommand(EMachineCommands::CREATE_RESOURCE);
    else
        PushCommand(EMachineCommands::UPDATE_RESOURCE);

    return resource.id;
}

Resource* VirtualMachine::CreateTexture2D(const Texture2DResourceDesc& desc)
{
    auto& resource = resourcesManager.CreateTexture2D(desc);
    //resource.resource = RenderDevice->CreateResource(resource.resourceDescription);
    //todo Command
    PushData((void*)resource.id);

    if (resource.resource == nullptr)
        PushCommand(EMachineCommands::CREATE_RESOURCE);
    else
        PushCommand(EMachineCommands::UPDATE_RESOURCE);


    return resource.id;
}

Resource* VirtualMachine::CreateTexture3D(const Texture3DResourceDesc& desc)
{
    auto& resource = resourcesManager.CreateTexture3D(desc);
    //resource.resource = RenderDevice->CreateResource(resource.resourceDescription);
    //todo Command


    PushData((void*)resource.id);

    if (resource.resource == nullptr)
        PushCommand(EMachineCommands::CREATE_RESOURCE);
    else
        PushCommand(EMachineCommands::UPDATE_RESOURCE);

    return resource.id;
}

Resource* VirtualMachine::CreateTextureCube(const TextureCubeResourceDesc& desc)
{
    auto& resource = resourcesManager.CreateTextureCube(desc);
    //resource.resource = RenderDevice->CreateResource(resource.resourceDescription);
    //todo Command

    PushData((void*)resource.id);

    if (resource.resource == nullptr)
        PushCommand(EMachineCommands::CREATE_RESOURCE);
    else
        PushCommand(EMachineCommands::UPDATE_RESOURCE);


    return resource.id;
}

VertexBuffer* VirtualMachine::CreateVertexBuffer(const BufferResourceDesc& desc)
{
    auto& resource = resourcesManager.CreateVertexBuffer(desc);
    //resource.resource = RenderDevice->CreateResource(resource.resourceDescription);
    //todo Command


    PushData((void*)resource.id);


    if (resource.resource == nullptr)
        PushCommand(EMachineCommands::CREATE_RESOURCE);
    else
        PushCommand(EMachineCommands::UPDATE_RESOURCE);
    return reinterpret_cast<VertexBuffer*>(resource.id);
}

ConstBuffer* VirtualMachine::CreateConstBuffer(const BufferResourceDesc& desc)
{
    auto& resource = resourcesManager.CreateConstBuffer(desc);
    //resource.resource = RenderDevice->CreateResource(resource.resourceDescription);
    //todo Command

    PushData((void*)resource.id);

    if (resource.resource == nullptr)
        PushCommand(EMachineCommands::CREATE_RESOURCE);
    else
        PushCommand(EMachineCommands::UPDATE_RESOURCE);

    return reinterpret_cast<ConstBuffer*>(resource.id);
}

IndexBuffer* VirtualMachine::CreateIndexBuffer(const BufferResourceDesc& desc)
{
    auto& resource = resourcesManager.CreateIndexBuffer(desc);
    //resource.resource = RenderDevice->CreateResource(resource.resourceDescription);
    //todo Command

    PushData((void*)resource.id);

    if (resource.resource == nullptr)
        PushCommand(EMachineCommands::CREATE_RESOURCE);
    else
        PushCommand(EMachineCommands::UPDATE_RESOURCE);

    return reinterpret_cast<IndexBuffer*>(resource.id);
}

ConstBufferView* VirtualMachine::CreateConstBufferView(const ConstBufferViewDesc& desc)
{
    auto& resourceView = resourcesManager.CreateConstBufferView(desc);
    //resourceView.view = RenderDevice->CreateConstBufferView(resourceView.cbViewDescription);

    PushData((void*)resourceView.id);
    PushCommand(EMachineCommands::CREATE_RESOURCE_VIEW);

    return reinterpret_cast<ConstBufferView*>(resourceView.id);
}

VertexBufferView* VirtualMachine::CreateVertexBufferView(const VertexBufferViewDesc& desc)
{
    auto& resourceView = resourcesManager.CreateVertexBufferView(desc);
    //resourceView.view = RenderDevice->CreateResourceView(resourceView);

    PushData((void*)resourceView.id);

    PushCommand(EMachineCommands::CREATE_RESOURCE_VIEW);
    return reinterpret_cast<VertexBufferView*>(resourceView.id);
}

IndexBufferView* VirtualMachine::CreateIndexBufferView(const IndexBufferViewDesc& desc)
{
    auto& resourceView = resourcesManager.CreateIndexBufferView(desc);
    //resourceView.view = RenderDevice->CreateResourceView(resourceView);

    PushData((void*)resourceView.id);

    PushCommand(EMachineCommands::CREATE_RESOURCE_VIEW);
    return reinterpret_cast<IndexBufferView*>(resourceView.id);
}

DepthStencilView* VirtualMachine::CreateDepthStencilView(const DepthStencilViewDesc& desc)
{
    auto& resourceView = resourcesManager.CreateDepthStencilView(desc);
    //resourceView.view = RenderDevice->CreateResourceView(resourceView);
    PushData((void*)resourceView.id);

    PushCommand(EMachineCommands::CREATE_RESOURCE_VIEW);

    return reinterpret_cast<DepthStencilView*>(resourceView.id);
}

ShaderResourceView* VirtualMachine::CreateShaderResourceView(const ShaderResourceViewDesc& desc)
{
    auto& resourceView = resourcesManager.CreateShaderResourceView(desc);
    //resourceView.view = RenderDevice->CreateResourceView(resourceView);

    PushData((void*)resourceView.id);

    PushCommand(EMachineCommands::CREATE_RESOURCE_VIEW);
    return reinterpret_cast<ShaderResourceView*>(resourceView.id);
}

RenderTargetView* VirtualMachine::CreateRenderTargetView(const RenderTargetViewDesc& desc)
{
    auto& resourceView = resourcesManager.CreateRenderTargetView(desc);
    //resourceView.view = RenderDevice->CreateResourceView(resourceView);

    PushData((void*)resourceView.id);
    PushCommand(EMachineCommands::CREATE_RESOURCE_VIEW);

    return reinterpret_cast<RenderTargetView*>(resourceView.id);
}

DepthStencilView* VirtualMachine::CreateDepthStencilsView(const DepthStencilViewDesc& description)
{
    auto& resourceView = resourcesManager.CreateDepthStencilsView(description);
    //resourceView.view = RenderDevice->CreateResourceView(resourceView);


    PushData((void*)resourceView.id);
    PushCommand(EMachineCommands::CREATE_RESOURCE_VIEW);

    return reinterpret_cast<DepthStencilView*>(resourceView.id);
}

UATargetView* VirtualMachine::CreateUATargetView(const UATargetViewDesc& desc)
{
    auto& resourceView = resourcesManager.CreateUATargetView(desc);
    //resourceView.view = RenderDevice->CreateResourceView(resourceView);

    PushData((void*)resourceView.id);
    PushCommand(EMachineCommands::CREATE_RESOURCE_VIEW);

    return reinterpret_cast<UATargetView*>(resourceView.id);
}

void VirtualMachine::AddDisposeResource(const Resource* resource)
{
    auto& gpuResource = resourcesManager.Resources[(uintptr_t)resource];
    for (auto& resourceView : gpuResource.views)
    {
        RenderDevice->DestroyResourceView(resourcesManager.GetRealResourceView(resourceView));
    }
    RenderDevice->DestroyResource(resourcesManager.GetRealResource(resource));
    resourcesManager.AddDisposeResource(resource);
}

void VirtualMachine::AddDisposeResourceView(const ResourceView* resourceView)
{
    RenderDevice->DestroyResourceView(resourcesManager.GetRealResourceView(resourceView));
    resourcesManager.AddDisposeResourceView(resourceView);
}

Shader* VirtualMachine::CreateShader(const ShaderDesc& desc)
{
    auto& shader = resourcesManager.CreateShader(desc);
    shader.shader = RenderDevice->CreateShader(shader.description);
    //PushData(shader.id);
    return reinterpret_cast<Shader*>(shader.id);
}

InputLayout* VirtualMachine::CreateInputLayout(const InputAssemblerDeclarationDesc& desc, const ShaderDesc& Shader)
{
    auto& inputLayout = resourcesManager.CreateInputLayout(desc);
    inputLayout.inputLayout = RenderDevice->CreateInputLayout(inputLayout.description, Shader);
    return reinterpret_cast<InputLayout*>(inputLayout.id);
}



void VirtualMachine::SetResourceData(Resource* resource, uint16_t dstSubresource, const UBox& rect,
    const void* pSrcData, int32_t srcRowPitch, int32_t srcDepthPitch)
{
    //PushData((void*)resource);
    auto& Resource = resourcesManager.GetResource(resource);

    uint32_t dataSize = (rect.Back - rect.Front) * (rect.Right - rect.Left) * (rect.Bottom - rect.Top)
        * (FormatByteSize[to_underlying(Resource.resourceDescription.Format)]);
    //+  ((Resource.resourceDescription.Dimension == EResourceDimension::RESOURCE_DIMENSION_BUFFER) ? 1 : 0));

    SetResourceDataDesc description =
    {
        resource,
        {rect,dstSubresource,srcRowPitch,srcDepthPitch,dataSize},
        dataQueue.size() + sizeof(SetResourceDataDesc)
    };

    PushData(description);
    PushData(pSrcData, dataSize);
    PushCommand(EMachineCommands::SET_RESOURCE_DATA);

    //
}

void VirtualMachine::UploadResourceData(Resource* resource, uint16_t dstSubresource, uint32_t dataSize, const void* pSrcData, int32_t srcRowPitch, int32_t srcDepthPitch)
{
    //PushData((void*)resource);
    auto& Resource = resourcesManager.GetResource(resource);
    //+  ((Resource.resourceDescription.Dimension == EResourceDimension::RESOURCE_DIMENSION_BUFFER) ? 1 : 0));

    UploadResourceDataDesc description =
    {
        resource,
        {dstSubresource,srcRowPitch,srcDepthPitch,dataSize},
        dataQueue.size() + sizeof(SetResourceDataDesc)
    };

    PushData(description);
    PushData(pSrcData, dataSize);
    PushCommand(EMachineCommands::UPLOAD_RESOURCE_DATA);
}

void VirtualMachine::SetVertexBufferData(VertexBuffer* vertexBuffer, const void* pSrcData, uint32_t dataLength, uint32_t offset,
    int32_t srcRowPitch, int32_t srcDepthPitch)
{
#ifdef _DEBUG
    if (offset == 0 && dataLength == resourcesManager.GetResource(vertexBuffer).resourceDescription.Width)
        UploadResourceData(vertexBuffer, 0, dataLength, pSrcData, srcRowPitch, srcDepthPitch);
    else
    {
        throw std::exception("Unsupported.");
    }
#else
        UploadResourceData(vertexBuffer, 0, dataLength, pSrcData, srcRowPitch, srcDepthPitch);
#endif
    
}

void VirtualMachine::SetIndexBufferData(IndexBuffer* buffer, const void* pSrcData, uint32_t dataLength, uint32_t offset,
    int32_t srcRowPitch, int32_t srcDepthPitch)
{
#ifdef _DEBUG
    if (offset == 0 && dataLength == resourcesManager.GetResource(buffer).resourceDescription.Width)
        UploadResourceData(buffer, 0, dataLength, pSrcData, srcRowPitch, srcDepthPitch);
    else
    {
        throw std::exception("Unsupported.");
    }
#else
    UploadResourceData(buffer, 0, dataLength, pSrcData, srcRowPitch, srcDepthPitch);
#endif
}

void VirtualMachine::SetConstBufferData(ConstBuffer* constBuffer, const void* data, uint32_t dataSize, uint32_t offset)
{
    UploadResourceData(constBuffer, 0, dataSize, data, 0, 0);
}



void VirtualMachine::ClearState()
{
    PushCommand(EMachineCommands::CLEAR_PIPELINE);
}
void VirtualMachine::ClearRenderTarget(RenderTargetView* rtView, FColor color)
{
    PushData(ClearRenderTargetDesc{rtView,color});
    PushCommand(EMachineCommands::CLEAR_RT);
}
void VirtualMachine::ClearDepthStencil(DepthStencilView* dsView, float depth, int8_t stencil)
{
    PushData(ClearDepthStencilDesc{dsView,depth,stencil});
    PushCommand(EMachineCommands::CLEAR_DS);
}


void* VirtualMachine::GetNativeTexture(const ShaderResourceView* shView)
{
    auto view = resourcesManager.GetRealResourceView(shView);
    return RenderDevice->GetNativeTexture(view);
}
