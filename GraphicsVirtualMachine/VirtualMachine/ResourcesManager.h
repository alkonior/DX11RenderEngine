#pragma once
#include <unordered_map>
#include <type_traits>
#include "BaseStructures.h"
#include "../../DX11RenderEngine/RenderEngine.h"
#include "CoreStructures/GPUResource.h"


namespace GVM
{
    
class ResourcesManager
{
    
    uint32_t freeIndex = 0;
    std::unordered_map<uint32_t, GpuResource> Resources = {};
    std::unordered_map<uint32_t, GpuResourceView> ResourceViews = {};

    uint32_t dataSize = 0;
    std::vector<uint8_t> savedData;

    uint32_t numRequiredUpdate = 0;

    RenderDevice* device;

    uint32_t CalculateResourceSize(const BufferResourceDesc& desc);
    uint32_t CalculateResourceSize(const TextureResourceDesc& desc);
    uint32_t CalculateResourceSize(const Texture1DResourceDesc& desc);
    uint32_t CalculateResourceSize(const Texture2DResourceDesc& desc);
    uint32_t CalculateResourceSize(const Texture3DResourceDesc& desc);
    uint32_t CalculateResourceSize(const TextureCubeResourceDesc& desc);
    
public:

    ResourcesManager(RenderDevice* device);
    
    const IResource* CreateBuffer           (const BufferResourceDesc& desc);
    const IResource* CreateTexture          (const TextureResourceDesc& desc);
    const IResource* CreateTexture1D        (const Texture1DResourceDesc& desc);
    const IResource* CreateTexture2D        (const Texture2DResourceDesc& desc);
    const IResource* CreateTexture3D        (const Texture3DResourceDesc& desc);
    const IResource* CreateTextureCube      (const TextureCubeResourceDesc& desc);
    const IVertexBuffer* CreateVertexBuffer (const BufferResourceDesc& desc);
    const IConstBuffer* CreateConstBuffer   (const BufferResourceDesc& desc);
    const IIndexBuffer* CreateIndexBuffer   (const BufferResourceDesc& desc);

    const IConstBufferView* CreateConstBufferView(const ConstBufferViewDesc& desc);
    const IResourceView* CreateDepthStencilView(const DepthStencilViewDesc& desc);
    const IRenderTargetView* CreateRenderTargetView(const RenderTargetViewDesc& desc);
    const IResourceView* CreateShaderResourceView(const ShaderResourceViewDesc& desc);
    const IVertexBufferView* CreateVertexBufferView(const VertexBufferViewDesc& desc);
    const IIndexBufferView* CreateIndexBufferView(const IndexBufferViewDesc& desc);
    
    void AddDisposeResource(const IResource* resource);
    void AddDisposeResourceView(const IResourceView* resourceView);
    
    
};
    
}
