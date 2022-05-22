#pragma once
#include <unordered_map>
#include <type_traits>
#include "BaseStructures.h"
#include "GPUResource.h"


namespace GVM
{

class VirtualMachine;
    
class ResourcesManager
{
    
    uintptr_t freeIndex = 0;
    std::unordered_map<uint32_t, GpuResource> Resources = {};
    std::unordered_map<uint32_t, GpuResourceView> ResourceViews = {};

    uint32_t dataSize = 0;
    std::vector<uint8_t> savedData;

    std::vector<Resource*> ResourcesRequiredUpdate;
    std::vector<ResourceView*> ResourceViewsRequiredUpdate;

    IRenderDevice* device;

    static uint32_t CalculateResourceSize(const BufferResourceDesc& desc);
    static uint32_t CalculateResourceSize(const ResourceDesc& desc);
    static uint32_t CalculateResourceSize(const Texture1DResourceDesc& desc);
    static uint32_t CalculateResourceSize(const Texture2DResourceDesc& desc);
    static uint32_t CalculateResourceSize(const Texture3DResourceDesc& desc);
    static uint32_t CalculateResourceSize(const TextureCubeResourceDesc& desc);

    friend VirtualMachine;


    ResourcesManager(IRenderDevice* device);
    
    GpuResource& CreateResource         (const ResourceDesc& desc);
    GpuResource& CreateBuffer           (const BufferResourceDesc& desc);
    GpuResource& CreateTexture1D        (const Texture1DResourceDesc& desc);
    GpuResource& CreateTexture2D        (const Texture2DResourceDesc& desc);
    GpuResource& CreateTexture3D        (const Texture3DResourceDesc& desc);
    GpuResource& CreateTextureCube      (const TextureCubeResourceDesc& desc);
    GpuResource& CreateVertexBuffer (const BufferResourceDesc& desc);
    GpuResource& CreateConstBuffer   (const BufferResourceDesc& desc);
    GpuResource& CreateIndexBuffer   (const BufferResourceDesc& desc);

    GpuResourceView& CreateConstBufferView(const ConstBufferViewDesc& desc);
    GpuResourceView& CreateVertexBufferView(const VertexBufferViewDesc& desc);
    GpuResourceView& CreateIndexBufferView(const IndexBufferViewDesc& desc);
    GpuResourceView& CreateDepthStencilView(const DepthStencilViewDesc& desc);
    GpuResourceView& CreateShaderResourceView(const ShaderResourceViewDesc& desc);
    GpuResourceView& CreateRenderTargetView(const RenderTargetViewDesc& desc);
    GpuResourceView& CreateUATargetView(const UATargetViewDesc& desc);


    //void UpdateResources();
    
    void AddDisposeResource(const Resource* resource);
    void AddDisposeResourceView(const ResourceView* resourceView);
    
    
};
    
}
