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
    GpuResourceView(IResourceView* view, const ConstBufferViewDesc& desc);
    GpuResourceView(IResourceView* view, const DepthStencilViewDesc& desc);
    GpuResourceView(IResourceView* view, const RenderTargetViewDesc& desc);
    GpuResourceView(IResourceView* view, const ShaderResourceViewDesc& desc);
    GpuResourceView(IResourceView* view, const VertexBufferViewDesc& desc);
    GpuResourceView(IResourceView* view, const IndexBufferViewDesc& desc);
    
    ~GpuResourceView() = default;
private:
    union
    {
        ConstBufferViewDesc    cbViewDescription;
        DepthStencilViewDesc   dsViewDescription;
        RenderTargetViewDesc   rtViewDescription;               
        ShaderResourceViewDesc shViewDescription;
        VertexBufferViewDesc   vViewDescription;
        IndexBufferViewDesc    iViewDescription;
    };

  

protected:

};
    
class GpuResource
{
public:
    IResource*                  resource;
    uint32_t                    resourceBindings;
    uint32_t                    resourceUsages;
    std::set<GpuResourceView>   views;
    ResourceDesc                ResourceDescription;
    
protected:
    
};

}