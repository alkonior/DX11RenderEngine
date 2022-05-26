#include "ResourcesManager.h"

#include <cassert>

using namespace GVM;

const uint32_t FormatByteSize[] = {
    1, /*FORMAT_UNKNOWN*/

    //4 chanel
    4 * 4, /*FORMAT_R32G32B32A32_TYPELESS,*/
    4 * 4, /*FORMAT_R32G32B32A32_FLOAT,*/
    4 * 4, /*FORMAT_R32G32B32A32_UINT,*/
    4 * 4, /*FORMAT_R32G32B32A32_SINT,*/

    4 * 2, /*FORMAT_R16G16B16A16_TYPELESS,*/
    4 * 2, /*FORMAT_R16G16B16A16_FLOAT,*/
    4 * 2, /*FORMAT_R16G16B16A16_UNORM,*/
    4 * 2, /*FORMAT_R16G16B16A16_UINT,*/
    4 * 2, /*FORMAT_R16G16B16A16_SNORM,*/
    4 * 2, /*FORMAT_R16G16B16A16_SINT,*/

    4, /*FORMAT_R10G10B10A2_TYPELESS,*/
    4, /*FORMAT_R10G10B10A2_UNORM,*/
    4, /*FORMAT_R10G10B10A2_UINT,*/

    //3 chanel*/
    3 * 4, /*FORMAT_R32G32B32_TYPELESS,*/
    3 * 4, /*FORMAT_R32G32B32_FLOAT,*/
    3 * 4, /*FORMAT_R32G32B32_UINT,*/
    3 * 4, /*FORMAT_R32G32B32_SINT,*/

    4, /*FORMAT_R8G8B8A8_TYPELESS,*/
    4, /*FORMAT_R8G8B8A8_UNORM,*/
    4, /*FORMAT_R8G8B8A8_UNORM_SRGB,*/
    4, /*FORMAT_R8G8B8A8_UINT,*/
    4, /*FORMAT_R8G8B8A8_SNORM,*/
    4, /*FORMAT_R8G8B8A8_SINT,*/

    //2 chanel*/
    2 * 4, /*FORMAT_R32G32_TYPELESS,*/
    2 * 4, /*FORMAT_R32G32_FLOAT,*/
    2 * 4, /*FORMAT_R32G32_UINT,*/
    2 * 4, /*FORMAT_R32G32_SINT,*/

    2 * 2, /*FORMAT_R16G16_TYPELESS,*/
    2 * 2, /*FORMAT_R16G16_FLOAT,*/
    2 * 2, /*FORMAT_R16G16_UNORM,*/
    2 * 2, /*FORMAT_R16G16_UINT,*/
    2 * 2, /*FORMAT_R16G16_SNORM,*/
    2 * 2, /*FORMAT_R16G16_SINT,*/

    2, /*FORMAT_R8G8_TYPELESS,*/
    2, /*FORMAT_R8G8_UNORM,*/
    2, /*FORMAT_R8G8_UINT,*/
    2, /*FORMAT_R8G8_SNORM,*/
    2, /*FORMAT_R8G8_SINT,*/

    //1 chanel*/
    4, /*FORMAT_R32_TYPELESS,*/
    4, /*FORMAT_R32_FLOAT,*/
    4, /*FORMAT_R32_UINT,*/
    4, /*FORMAT_R32_SINT,*/

    2, /*FORMAT_R16_TYPELESS,*/
    2, /*FORMAT_R16_FLOAT,*/
    2, /*FORMAT_R16_UNORM,*/
    2, /*FORMAT_R16_UINT,*/
    2, /*FORMAT_R16_SNORM,*/
    2, /*FORMAT_R16_SINT,*/

    1, /*FORMAT_R8_TYPELESS,*/
    1, /*FORMAT_R8_UNORM,*/
    1, /*FORMAT_R8_UINT,*/
    1, /*FORMAT_R8_SNORM,*/
    1, /*FORMAT_R8_SINT,*/
    1 /*FORMAT_A8_UNORM,*/
};


uint32_t ResourcesManager::CalculateResourceSize(const ResourceDesc& desc)
{
    //uint8_t numLevels = desc.MipLevels;
    //if (!numLevels)
    //    numLevels = 1 + floor(log2(std::max(desc.Height, desc.Height, desc.DepthOrArraySize)));
    uint32_t width = desc.Width;
    uint32_t height = desc.Height;
    uint32_t depth = desc.DepthOrArraySize;
    uint32_t formatSize = FormatByteSize[to_underlying(desc.Format)];

    uint32_t size = width * height * depth * formatSize;
    assert(size != 0);
    //for (numLevels)
    //{
    //    
    //}

    return size;
}

uint32_t ResourcesManager::CalculateResourceSize(const BufferResourceDesc& desc)
{
    assert(desc.Size != 0);
    return desc.Size;
}

uint32_t ResourcesManager::CalculateResourceSize(const Texture1DResourceDesc& desc)
{
    ResourceDesc nDesc;
    nDesc.Format = desc.Format;
    nDesc.Width = desc.Width;
    nDesc.Height = 1;
    nDesc.DepthOrArraySize = desc.Array;
    //nDesc.MipLevels = desc.MipLevels;

    return CalculateResourceSize(nDesc);
}

uint32_t ResourcesManager::CalculateResourceSize(const Texture2DResourceDesc& desc)
{
    ResourceDesc nDesc;
    nDesc.Format = desc.Format;
    nDesc.Width = desc.Width;
    nDesc.Height = 1;
    nDesc.DepthOrArraySize = desc.Array;
    //nDesc.MipLevels = desc.MipLevels;

    return CalculateResourceSize(nDesc);
}

uint32_t ResourcesManager::CalculateResourceSize(const Texture3DResourceDesc& desc)
{
    ResourceDesc nDesc;
    nDesc.Format = desc.Format;
    nDesc.Width = desc.Width;
    nDesc.Height = 1;
    nDesc.DepthOrArraySize = desc.Depth;
    //nDesc.MipLevels = desc.MipLevels;

    return CalculateResourceSize(nDesc);
}

uint32_t ResourcesManager::CalculateResourceSize(const TextureCubeResourceDesc& desc)
{
    ResourceDesc nDesc;
    nDesc.Format = desc.Format;
    nDesc.Width = desc.Width;
    nDesc.Height = 1;
    nDesc.DepthOrArraySize = desc.Array;
    //nDesc.MipLevels = desc.MipLevels;

    return CalculateResourceSize(nDesc);
}

//ResourcesManager::ResourcesManager(IRenderDevice* device) : device(device){}


GpuResource& ResourcesManager::CreateResource(const ResourceDesc& desc)
{
    uintptr_t resourceIndex = freeIndex++;
    Resource* resourcePtr = reinterpret_cast<Resource*>(resourceIndex);
    ResourceDesc savedDesc = desc;

    if (savedDesc.initialData)
    {
        const uint32_t resourceSize = CalculateResourceSize(desc);
        savedData.resize(savedData.size() + resourceSize, 0);
        memcpy(savedData.data() + dataSize, savedDesc.initialData, resourceSize);
        savedDesc.initialData = savedData.data() + dataSize;
        dataSize += resourceSize;
    }

    Resources[resourceIndex] = GpuResource(resourcePtr, savedDesc);
    ResourcesRequiredUpdate.push_back(resourcePtr);
    return Resources.at(resourceIndex);
}


GpuResource& ResourcesManager::CreateBuffer(const BufferResourceDesc& desc)
{
    ResourceDesc savedDesc;
    savedDesc.Dimension = EResourceDimension::RESOURCE_DIMENSION_BUFFER;
    savedDesc.Format = EFormat::FORMAT_UNKNOWN;
    savedDesc.Width = desc.Size;
    savedDesc.Height = 1;
    savedDesc.DepthOrArraySize = 1;
    //savedDesc.MipLevels = desc.MipLevels; //todo
    savedDesc.initialData = desc.initialData;

    return CreateResource(savedDesc);
}

GpuResource& ResourcesManager::CreateTexture1D(const Texture1DResourceDesc& desc)
{
    ResourceDesc savedDesc;
    savedDesc.Dimension = EResourceDimension::RESOURCE_DIMENSION_TEXTURE1D;
    savedDesc.Format = desc.Format;
    savedDesc.Width = desc.Width;
    savedDesc.Height = 1;
    savedDesc.DepthOrArraySize = 1;
    //savedDesc.MipLevels = desc.MipLevels; //todo
    savedDesc.initialData = desc.initialData;

    return CreateResource(savedDesc);
}

GpuResource& ResourcesManager::CreateTexture2D(const Texture2DResourceDesc& desc)
{
    ResourceDesc savedDesc;
    savedDesc.Dimension = EResourceDimension::RESOURCE_DIMENSION_TEXTURE2D;
    savedDesc.Format = desc.Format;
    savedDesc.Width = desc.Width;
    savedDesc.Height = desc.Height;
    savedDesc.DepthOrArraySize = desc.Array;
    //savedDesc.MipLevels = desc.MipLevels; //todo
    savedDesc.initialData = desc.initialData;

    return CreateResource(savedDesc);
}

GpuResource& ResourcesManager::CreateTexture3D(const Texture3DResourceDesc& desc)
{
    ResourceDesc savedDesc;
    savedDesc.Dimension = EResourceDimension::RESOURCE_DIMENSION_TEXTURE3D;
    savedDesc.Format = desc.Format;
    savedDesc.Width = desc.Width;
    savedDesc.Height = desc.Height;
    savedDesc.DepthOrArraySize = desc.Depth;
    //savedDesc.MipLevels = desc.MipLevels; //todo
    savedDesc.initialData = desc.initialData;

    return CreateResource(savedDesc);
}

GpuResource& ResourcesManager::CreateTextureCube(const TextureCubeResourceDesc& desc)
{
    ResourceDesc savedDesc;
    savedDesc.Dimension = EResourceDimension::RESOURCE_DIMENSION_TEXTURE2D;
    savedDesc.Format = desc.Format;
    savedDesc.Width = desc.Width;
    savedDesc.Height = desc.Height;
    savedDesc.DepthOrArraySize = desc.Array * 6;
    //savedDesc.MipLevels = desc.MipLevels; //todo
    savedDesc.initialData = desc.initialData;

    return CreateResource(savedDesc);
}

GpuResource& ResourcesManager::CreateVertexBuffer(const BufferResourceDesc& desc)
{
    ResourceDesc savedDesc;
    savedDesc.Dimension = EResourceDimension::RESOURCE_DIMENSION_BUFFER;
    savedDesc.Format = EFormat::FORMAT_UNKNOWN;
    savedDesc.Width = desc.Size;
    savedDesc.Height = 1;
    savedDesc.DepthOrArraySize = 1;
    //savedDesc.MipLevels = desc.MipLevels; //todo
    savedDesc.initialData = desc.initialData;

    return CreateResource(savedDesc);
}

GpuResource& ResourcesManager::CreateConstBuffer(const BufferResourceDesc& desc)
{
    ResourceDesc savedDesc;
    savedDesc.Dimension = EResourceDimension::RESOURCE_DIMENSION_BUFFER;
    savedDesc.Format = EFormat::FORMAT_UNKNOWN;
    savedDesc.Width = desc.Size;
    savedDesc.Height = 1;
    savedDesc.DepthOrArraySize = 1;
    //savedDesc.MipLevels = desc.MipLevels; //todo
    savedDesc.initialData = desc.initialData;

    return CreateResource(savedDesc);
}

GpuResource& ResourcesManager::CreateIndexBuffer(const BufferResourceDesc& desc)
{
    ResourceDesc savedDesc;
    savedDesc.Dimension = EResourceDimension::RESOURCE_DIMENSION_BUFFER;
    savedDesc.Format = EFormat::FORMAT_UNKNOWN;
    savedDesc.Width = desc.Size;
    savedDesc.Height = 1;
    savedDesc.DepthOrArraySize = 1;
    //savedDesc.MipLevels = desc.MipLevels; //todo
    savedDesc.initialData = desc.initialData;

    return CreateResource(savedDesc);
}

GpuResourceView& ResourcesManager::CreateConstBufferView(const ConstBufferViewDesc& desc)
{
    auto resId = reinterpret_cast<uintptr_t>(desc.Buffer);
    auto resPtr = reinterpret_cast<Resource*>(desc.Buffer);

#ifdef _DEBUG
    assert(desc.Buffer != nullptr);
    assert(Resources.contains(resId));
#endif

    auto& resource = Resources.at(resId);

#ifdef _DEBUG
    assert(resource.resourceDescription.Dimension == EResourceDimension::RESOURCE_DIMENSION_BUFFER);
    assert(resource.resourceDescription.Width >= desc.Size);
#endif

    if (!(resource.resourceBindings & to_underlying(EBindFlags::BIND_CONSTANT_BUFFER)))
    {
        resource.resourceBindings |= to_underlying(EBindFlags::BIND_CONSTANT_BUFFER);
        if (!resource.isRequiredUpdate)
            ResourcesRequiredUpdate.push_back(resPtr);
        resource.isRequiredUpdate = true;
    }


    uintptr_t viewIndex = freeIndex++;
    ConstBufferView* viewPtr = reinterpret_cast<ConstBufferView*>(viewIndex);
    ResourceViews.insert(std::make_pair(viewIndex, GpuResourceView(viewPtr, desc)));

    resource.views.push_back(viewPtr);
    return ResourceViews.at(viewIndex);
}

GpuResourceView& ResourcesManager::CreateVertexBufferView(const VertexBufferViewDesc& desc)
{
    auto resId = reinterpret_cast<uintptr_t>(desc.Buffer);
    auto resPtr = reinterpret_cast<Resource*>(desc.Buffer);
#ifdef _DEBUG
    assert(desc.Buffer != nullptr);
    assert(Resources.contains(resId));
#endif

    auto& resource = Resources.at(resId);

#ifdef _DEBUG
    assert(resource.resourceDescription.Dimension == EResourceDimension::RESOURCE_DIMENSION_BUFFER);
    assert(resource.resourceDescription.Width >= desc.Size);
#endif

    if (!(resource.resourceBindings & to_underlying(EBindFlags::BIND_VERTEX_BUFFER)))
    {
        resource.resourceBindings |= to_underlying(EBindFlags::BIND_VERTEX_BUFFER);
        if (!resource.isRequiredUpdate)
            ResourcesRequiredUpdate.push_back(resPtr);
        resource.isRequiredUpdate = true;
    }


    uintptr_t viewIndex = freeIndex++;
    VertexBufferView* viewPtr = reinterpret_cast<VertexBufferView*>(viewIndex);
    ResourceViews.insert(std::make_pair(viewIndex, GpuResourceView(viewPtr, desc)));

    resource.views.push_back(viewPtr);
    return ResourceViews.at(viewIndex);
}

GpuResourceView& ResourcesManager::CreateIndexBufferView(const IndexBufferViewDesc& desc)
{
    auto resId = reinterpret_cast<uintptr_t>(desc.Buffer);
    auto resPtr = reinterpret_cast<Resource*>(desc.Buffer);
#ifdef _DEBUG
    assert(desc.Buffer != nullptr);
    assert(Resources.contains(resId));
#endif

    auto& resource = Resources.at(resId);

#ifdef _DEBUG
    assert(resource.resourceDescription.Dimension == EResourceDimension::RESOURCE_DIMENSION_BUFFER);
    assert(resource.resourceDescription.Width >= desc.Size);
#endif

    if (!(resource.resourceBindings & to_underlying(EBindFlags::BIND_INDEX_BUFFER)))
    {
        resource.resourceBindings |= to_underlying(EBindFlags::BIND_INDEX_BUFFER);
        if (!resource.isRequiredUpdate)
            ResourcesRequiredUpdate.push_back(resPtr);
        resource.isRequiredUpdate = true;
    }


    uintptr_t viewIndex = freeIndex++;
    IndexBufferView* viewPtr = reinterpret_cast<IndexBufferView*>(viewIndex);
    ResourceViews.insert(std::make_pair(viewIndex, GpuResourceView(viewPtr, desc)));

    resource.views.push_back(viewPtr);
    return ResourceViews.at(viewIndex);
}

GpuResourceView& ResourcesManager::CreateDepthStencilView(const DepthStencilViewDesc& desc)
{
    auto resId = reinterpret_cast<uintptr_t>(desc.Resource);
    auto resPtr = reinterpret_cast<Resource*>(desc.Resource);
#ifdef _DEBUG
    assert(desc.Resource != nullptr);
    assert(Resources.contains(resId));
#endif

    auto& resource = Resources.at(resId);

#ifdef _DEBUG
    assert(resource.resourceDescription.Dimension == EResourceDimension::RESOURCE_DIMENSION_TEXTURE2D);
#endif

    if (!(resource.resourceBindings & to_underlying(EBindFlags::BIND_DEPTH_STENCIL)))
    {
        resource.resourceBindings |= to_underlying(EBindFlags::BIND_DEPTH_STENCIL);
        if (!resource.isRequiredUpdate)
            ResourcesRequiredUpdate.push_back(resPtr);
        resource.isRequiredUpdate = true;
    }


    uintptr_t viewIndex = freeIndex++;
    DepthStencilView* viewPtr = reinterpret_cast<DepthStencilView*>(viewIndex);
    ResourceViews.insert(std::make_pair(viewIndex, GpuResourceView(viewPtr, desc)));

    resource.views.push_back(viewPtr);
    return ResourceViews.at(viewIndex);
}

const EResourceDimension ShaderViewDimToResDim[] = {
    EResourceDimension::RESOURCE_DIMENSION_UNKNOWN, /*DIMENSION_UNKNOWN = 0,*/
    EResourceDimension::RESOURCE_DIMENSION_TEXTURE1D, /*DIMENSION_TEXTURE1D,*/
    EResourceDimension::RESOURCE_DIMENSION_TEXTURE1D, /*DIMENSION_TEXTURE1D_ARRAY,*/
    EResourceDimension::RESOURCE_DIMENSION_TEXTURE2D, /*DIMENSION_TEXTURE2D,*/
    EResourceDimension::RESOURCE_DIMENSION_TEXTURE2D, /*DIMENSION_TEXTURE2D_ARRAY,*/
    EResourceDimension::RESOURCE_DIMENSION_TEXTURE3D, /*DIMENSION_TEXTURE3D,*/
    EResourceDimension::RESOURCE_DIMENSION_TEXTURE2D, /*DIMENSION_TEXTURECUBE,*/
    EResourceDimension::RESOURCE_DIMENSION_TEXTURE2D, /*DIMENSION_TEXTURECUBE_ARRAY,*/
};

GpuResourceView& ResourcesManager::CreateShaderResourceView(const ShaderResourceViewDesc& desc)
{
    auto resId = reinterpret_cast<uintptr_t>(desc.Resource);
    auto resPtr = reinterpret_cast<Resource*>(desc.Resource);
#ifdef _DEBUG
    assert(desc.Resource != nullptr);
    assert(Resources.contains(resId));
#endif

    auto& resource = Resources.at(resId);

#ifdef _DEBUG
    assert(desc.Dimension != EShaderViewDimension::DIMENSION_UNKNOWN);
    assert(desc.Format != EFormat::FORMAT_UNKNOWN);
    assert(resource.resourceDescription.Dimension == ShaderViewDimToResDim[to_underlying(desc.Dimension)]);
    assert(FormatByteSize[to_underlying(resource.resourceDescription.Format)] ==
        FormatByteSize[to_underlying(desc.Format)]);
#endif

    if (!(resource.resourceBindings & to_underlying(EBindFlags::BIND_SHADER_RESOURCE)))
    {
        resource.resourceBindings |= to_underlying(EBindFlags::BIND_SHADER_RESOURCE);
        if (!resource.isRequiredUpdate)
            ResourcesRequiredUpdate.push_back(resPtr);
        resource.isRequiredUpdate = true;
    }


    uintptr_t viewIndex = freeIndex++;
    ShaderResourceView* viewPtr = reinterpret_cast<ShaderResourceView*>(viewIndex);
    ResourceViews.insert(std::make_pair(viewIndex, GpuResourceView(viewPtr, desc)));

    resource.views.push_back(viewPtr);
    return ResourceViews.at(viewIndex);
}

const EResourceDimension RTViewDimToResDim[] = {
    EResourceDimension::RESOURCE_DIMENSION_UNKNOWN, /* DIMENSION_UNKNOWN = 0,*/
    /*EResourceDimension::RESOURCE_DIMENSION_BUFFER, //DIMENSION_BUFFER, TODO unsupported*/
    EResourceDimension::RESOURCE_DIMENSION_TEXTURE1D, /* DIMENSION_TEXTURE1D,*/
    EResourceDimension::RESOURCE_DIMENSION_TEXTURE1D, /* DIMENSION_TEXTURE1D_ARRAY,*/
    EResourceDimension::RESOURCE_DIMENSION_TEXTURE2D, /* DIMENSION_TEXTURE2D,*/
    EResourceDimension::RESOURCE_DIMENSION_TEXTURE2D, /* DIMENSION_TEXTURE2D_ARRAY,*/
    EResourceDimension::RESOURCE_DIMENSION_TEXTURE3D, /* DIMENSION_TEXTURE3D,*/
};

GpuResourceView& ResourcesManager::CreateRenderTargetView(const RenderTargetViewDesc& desc)
{
    auto resId = reinterpret_cast<uintptr_t>(desc.Resource);
    auto resPtr = reinterpret_cast<Resource*>(desc.Resource);
#ifdef _DEBUG
    assert(desc.Resource != nullptr);
    assert(Resources.contains(resId));
#endif

    auto& resource = Resources.at(resId);

#ifdef _DEBUG
    assert(desc.Dimension != ERTViewDimension::DIMENSION_UNKNOWN);
    assert(desc.Format != EFormat::FORMAT_UNKNOWN);
    assert(resource.resourceDescription.Dimension == ShaderViewDimToResDim[to_underlying(desc.Dimension)]);
    assert(FormatByteSize[to_underlying(resource.resourceDescription.Format)] ==
        FormatByteSize[to_underlying(desc.Format)]);
#endif

    if (!(resource.resourceBindings & to_underlying(EBindFlags::BIND_RENDER_TARGET)))
    {
        resource.resourceBindings |= to_underlying(EBindFlags::BIND_RENDER_TARGET);
        if (!resource.isRequiredUpdate)
            ResourcesRequiredUpdate.push_back(resPtr);
        resource.isRequiredUpdate = true;
    }


    uintptr_t viewIndex = freeIndex++;
    RenderTargetView* viewPtr = reinterpret_cast<RenderTargetView*>(viewIndex);
    ResourceViews.insert(std::make_pair(viewIndex, GpuResourceView(viewPtr, desc)));

    resource.views.push_back(viewPtr);
    return ResourceViews.at(viewIndex);
}

GpuResourceView& ResourcesManager::CreateUATargetView(const UATargetViewDesc& desc)
{
    auto resId = reinterpret_cast<uintptr_t>(desc.Resource);
    auto resPtr = reinterpret_cast<Resource*>(desc.Resource);
#ifdef _DEBUG
    assert(desc.Resource != nullptr);
    assert(Resources.contains(resId));
#endif

    auto& resource = Resources.at(resId);

#ifdef _DEBUG
    assert(desc.Dimension != ERTViewDimension::DIMENSION_UNKNOWN);
    assert(desc.Format != EFormat::FORMAT_UNKNOWN);
    assert(resource.resourceDescription.Dimension == ShaderViewDimToResDim[to_underlying(desc.Dimension)]);
    assert(FormatByteSize[to_underlying(resource.resourceDescription.Format)] ==
        FormatByteSize[to_underlying(desc.Format)]);
#endif

    if (!(resource.resourceBindings & to_underlying(EBindFlags::BIND_UNORDERED_ACCESS)))
    {
        resource.resourceBindings |= to_underlying(EBindFlags::BIND_UNORDERED_ACCESS);
        if (!resource.isRequiredUpdate)
            ResourcesRequiredUpdate.push_back(resPtr);
        resource.isRequiredUpdate = true;
    }


    uintptr_t viewIndex = freeIndex++;
    UATargetView* viewPtr = reinterpret_cast<UATargetView*>(viewIndex);
    ResourceViews.insert(std::make_pair(viewIndex, GpuResourceView(viewPtr, desc)));

    resource.views.push_back(viewPtr);
    return ResourceViews.at(viewIndex);
}


GPUShader& ResourcesManager::CreateShader(const ShaderDesc& desc)
{
    int shaderIndex = SavedShaders.size();
    ShaderDesc savedDesc = desc;

    assert(desc.bytecode);
#ifdef _DEBUG
    assert(desc.byteCodeSize);
    assert(desc.type != EShaderType::UNKNOWN);
#endif


    savedData.resize(savedData.size() + desc.byteCodeSize, 0);
    memcpy(savedData.data() + dataSize, savedDesc.bytecode, desc.byteCodeSize);
    savedDesc.bytecode = savedData.data() + dataSize;
    dataSize += desc.byteCodeSize;

    SavedShaders.push_back(GPUShader(savedDesc, shaderIndex));

    return SavedShaders[shaderIndex];
}

GPUInputLayout& ResourcesManager::CreateInputLayout(const InputAssemblerDeclarationDesc& desc)
{
    int index = SavedInputLayouts.size();

    SavedInputLayouts.push_back(GPUInputLayout(desc, index));
    return SavedInputLayouts[index];
}

IRenderDevice::IResource* ResourcesManager::GetRealResource(const Resource* resource)
{
    return Resources[reinterpret_cast<uint32_t>(resource)].resource;
}

IRenderDevice::IResourceView* ResourcesManager::GetRealResourceView(const ResourceView* resourceView)
{
    return ResourceViews[reinterpret_cast<uint32_t>(resourceView)].view;
}

IRenderDevice::IShader* ResourcesManager::GetRealShader(const Shader* shader)
{
    return SavedShaders[reinterpret_cast<uint32_t>(shader)].shader;
}

IRenderDevice::IInputLayout* ResourcesManager::GetRealInputLayout(const InputLayout* inputLayout)
{
    return SavedInputLayouts[reinterpret_cast<uint32_t>(inputLayout)].inputLayout;
}


void ResourcesManager::AddDisposeResource(const Resource* resource)
{}

void ResourcesManager::AddDisposeResourceView(const ResourceView* resourceView)
{}
