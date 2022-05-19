#pragma once
#include <set>
#include "BaseStructures.h"

namespace GVM
{

class GpuResourceView
{
    enum class EViewType
    {
        CB,
        DB,
        RTB,
        SRB,
        VB,
        IB,
    };
public:
    IResourceView* view;
    EViewType      type;
    bool           isRequiredUpdate;
    
    GpuResourceView(IResourceView* view, const ConstBufferViewDesc& desc);
    GpuResourceView(IResourceView* view, const DepthStencilViewDesc& desc);
    GpuResourceView(IResourceView* view, const RenderTargetViewDesc& desc);
    GpuResourceView(IResourceView* view, const ShaderResourceViewDesc& desc);
    GpuResourceView(IResourceView* view, const VertexBufferViewDesc& desc);
    GpuResourceView(IResourceView* view, const IndexBufferViewDesc& desc);
    
    GpuResourceView(const GpuResourceView& copy) noexcept;
    GpuResourceView(const GpuResourceView&& copy) noexcept;
    
    ~GpuResourceView() = default;
    
    union
    {
        ConstBufferViewDesc    cbViewDescription;
        DepthStencilViewDesc   dbViewDescription;
        RenderTargetViewDesc   rtViewDescription;               
        ShaderResourceViewDesc srViewDescription;
        VertexBufferViewDesc   vbViewDescription;
        IndexBufferViewDesc    ibViewDescription;
    };

  

protected:

};


class GpuResource
{
    enum class EResourceType
    {
        BR,
        TR,
    };
    
public:
    IResource*                  resource;
    uint32_t                    resourceBindings = 0;
    uint32_t                    resourceUsages   = 0;
    bool                        isRequiredUpdate = true;
    EResourceType               type;
    
    std::vector<IResourceView*>    views;
    
    GpuResource(IResource* resource, const BufferResourceDesc& desc);
    GpuResource(IResource* resource, const TextureResourceDesc& desc);

    
    GpuResource(const GpuResource& copy) noexcept;
    GpuResource(const GpuResource&& copy) noexcept;
    
    union 
    {
        BufferResourceDesc  bufferDescription;
        TextureResourceDesc textureDescription;
    };

    
    
protected:
    
};

}