#pragma once
#include <set>
#include "IRenderDevice.h"

namespace GVM {


class GpuResourceView {
public:
    
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
    
    ResourceView* id = nullptr;
    IRenderDevice::RESOURCEHANDLE realResource = nullptr;//todo
    IRenderDevice::RESOURCEVIEWHANDLE view = 0;
    Resource* resource = nullptr;
    EViewType type = EViewType::UNKNOWN;
    bool isRequiredUpdate = true;

    GpuResourceView(): rtViewDescription({nullptr, false, EFormat::FORMAT_R8G8B8A8_UNORM}) {};
    GpuResourceView(ResourceView* id, const ConstBufferViewDesc& desc);
    GpuResourceView(ResourceView* id, const DepthStencilViewDesc& desc);
    GpuResourceView(ResourceView* id, const RenderTargetViewDesc& desc);
    GpuResourceView(ResourceView* id, const UATargetViewDesc& desc);
    GpuResourceView(ResourceView* id, const ShaderResourceViewDesc& desc);
    GpuResourceView(ResourceView* id, const VertexBufferViewDesc& desc);
    GpuResourceView(ResourceView* id, const IndexBufferViewDesc& desc);

    GpuResourceView(const GpuResourceView& copy) noexcept;
    GpuResourceView& operator=(const GpuResourceView& copy) noexcept;
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
        uint64_t data[3];
    } ;



protected:
};


class GpuResource {

public:
    enum class ResourceState : uint8_t
    {
        RESOURCE_STATE_COMMON,
        RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
        RESOURCE_STATE_INDEX_BUFFER,
        RESOURCE_STATE_RENDER_TARGET,
        RESOURCE_STATE_UNORDERED_ACCESS,
        RESOURCE_STATE_DEPTH_WRITE,
        RESOURCE_STATE_DEPTH_READ,
        RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
        RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        RESOURCE_STATE_STREAM_OUT,
        RESOURCE_STATE_INDIRECT_ARGUMENT,
        RESOURCE_STATE_COPY_DEST,
        RESOURCE_STATE_COPY_SOURCE,
        RESOURCE_STATE_RESOLVE_DEST,
        RESOURCE_STATE_RESOLVE_SOURCE,
        RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
        RESOURCE_STATE_SHADING_RATE_SOURCE,
        RESOURCE_STATE_GENERIC_READ,
        RESOURCE_STATE_PRESENT,
        RESOURCE_STATE_ALL_SHADER_RESOURCE,
        RESOURCE_STATE_UNDEFINED,
        
    }; 
    
    Resource* id = nullptr;
    IRenderDevice::RESOURCEHANDLE resource = nullptr;
    uint32_t resourceBindings = 0;
    bool isRequiredUpdate = true;
    ResourceState currentState = ResourceState::RESOURCE_STATE_COMMON;
    ResourceState realState = ResourceState::RESOURCE_STATE_COMMON;

    std::vector<ResourceView*> views;

    GpuResource() = default;
    GpuResource(Resource* id, const ResourceDesc& desc);

    ResourceDesc resourceDescription;

protected:
};

struct ResourceStateTransition {
    enum : uint8_t {
        BEGIN = 1,
        END = 2,
        READ = 4,
        WRITE = 8
    };
    static constexpr uint8_t DefReadFlag = BEGIN | END | READ;
    static constexpr uint8_t DefWriteFlag = BEGIN | END | WRITE;
    static constexpr uint8_t DefInvalidFlag = 1 | 2;
    
    ResourceStateTransition(
        int8_t flags,
        Resource* resource,
        GpuResource::ResourceState StateFrom,
        GpuResource::ResourceState StateTo
    ) : flags(flags), resource(resource), StateFrom(StateFrom), StateTo(StateTo) {};
    
    Resource*  resource;
    uint8_t flags;// 1 - begin, 2 end, 3 both, 4 - read, 8 - write
    GpuResource::ResourceState StateFrom;
    GpuResource::ResourceState StateTo;
    
    inline bool operator==(const ResourceStateTransition& rhs) const
    {
        return
        rhs.resource == resource
        && (rhs.flags == flags)
        && rhs.StateFrom == StateFrom
        && rhs.StateTo == StateTo;
    }
};

inline ResourceStateTransition MakeTransition(
        GpuResource& resource,
        uint8_t flags,
        GpuResource::ResourceState StateTo)
{
    auto state = resource.currentState;
    resource.currentState = StateTo;
    return ResourceStateTransition(
    flags,
    resource.id,
    state,
    StateTo);
}

}
