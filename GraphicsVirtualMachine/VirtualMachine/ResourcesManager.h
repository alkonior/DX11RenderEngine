#pragma once
#include <unordered_map>
#include <type_traits>
#include "BaseStructures.h"
#include "CoreStructures/GPUResource.h"


namespace GVM
{
    
class ResourcesManager
{
    uint32_t freeIndex = 0;
    std::unordered_map<uint32_t, GpuResource> Resources = {};
    std::unordered_map<uint32_t, GpuResourceView> ResourceViews = {};
    
public:
    IResource* CreateBuffer(const BufferResourceDesc& desc);
    IResource* CreateTexture(const TextureResourceDesc& desc);
    IResource* CreateTexture1D(const Texture1DResourceDesc& desc);
    IResource* CreateTexture2D(const Texture2DResourceDesc& desc);
    IResource* CreateTexture3D(const Texture3DResourceDesc& desc);
    IResource* CreateTextureCube(const TextureCubeResourceDesc& desc);
    IResource* CreateVertexBuffer(const ShaderResourceViewDesc& desc);
    IResource* CreateConstBuffer(const ConstBufferDesc& desc);
    IResource* CreateIndexBuffer(const IndexBufferDesc& desc);

    IConstBufferView* CreateConstBufferView(const ConstBufferViewDesc& desc);
    IResourceView* CreateDepthStencilView(const DepthStencilViewDesc& desc);
    IRenderTargetView* CreateRenderTargetView(const RenderTargetViewDesc& desc);
    IResourceView* CreateShaderResourceView(const ShaderResourceViewDesc& desc);
    IVertexBufferView* CreateVertexBufferView(const VertexBufferViewDesc& desc);
    IIndexBufferView* CreateIndexBufferView(const IndexBufferViewDesc& desc);
    
};
    
}
