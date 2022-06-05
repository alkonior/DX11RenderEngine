#pragma once
#include <set>
#include "../IRenderDevice.h"

namespace GVM {


class GpuResourceView {
    enum class EViewType : uint8_t {
        UNKNOWN,
        CB,
        DB,
        RT,
        UA,
        SR,
        VB,
        IB,
    };
public:
    ResourceView* id = nullptr;
    IRenderDevice::IResourceView* view = nullptr;
    EViewType type = EViewType::UNKNOWN;
    bool isRequiredUpdate = true;

    GpuResourceView(){};
    GpuResourceView(ResourceView* id, const ConstBufferViewDesc& desc);
    GpuResourceView(ResourceView* id, const DepthStencilViewDesc& desc);
    GpuResourceView(ResourceView* id, const RenderTargetViewDesc& desc);
    GpuResourceView(ResourceView* id, const UATargetViewDesc& desc);
    GpuResourceView(ResourceView* id, const ShaderResourceViewDesc& desc);
    GpuResourceView(ResourceView* id, const VertexBufferViewDesc& desc);
    GpuResourceView(ResourceView* id, const IndexBufferViewDesc& desc);

    GpuResourceView(const GpuResourceView& copy) noexcept;
    GpuResourceView(const GpuResourceView&& copy) noexcept;

    ~GpuResourceView() = default;

    union {
        ConstBufferViewDesc cbViewDescription;
        DepthStencilViewDesc dbViewDescription;
        RenderTargetViewDesc rtViewDescription;
        UATargetViewDesc uaViewDescription;
        ShaderResourceViewDesc srViewDescription;
        VertexBufferViewDesc vbViewDescription;
        IndexBufferViewDesc ibViewDescription;
    };



protected:
};


class GpuResource {

public:
    Resource* id = nullptr;
    IRenderDevice::IResource* resource = nullptr;
    uint32_t resourceBindings = 0;
    bool isRequiredUpdate = true;

    std::vector<ResourceView*> views;

    GpuResource() = default;
    GpuResource(Resource* id, const ResourceDesc& desc);

    ResourceDesc resourceDescription;

protected:
};

}
