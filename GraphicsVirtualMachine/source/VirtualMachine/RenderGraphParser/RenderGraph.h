#pragma once
#include <memory>

#include "../PipelineSnapshot.h"
#include "IRenderDevice.h"

namespace GVM {



struct RenderGraphNode : public std::tuple<EMachineCommands, void*> {
    RenderGraphNode(
    EMachineCommands Command,
    void* Description
    //std::vector<Resource*>&& ReadDependencies,
    //std::vector<Resource*>&& WrightDependencies
    );

    auto& Command() const
    {
        return std::get<0>(*this);
    }
    auto& Description() const
    {
        return std::get<1>(*this);
    }
    

    
    //EMachineCommands Command;
    //void* Description;

    //std::vector<Resource*> ReadDependencies;
    //std::vector<Resource*> WrightDependencies;
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
        IRenderDevice::RESOURCEHANDLE resource,
        GpuResource::ResourceState StateFrom,
        GpuResource::ResourceState StateTo
    ) : flags(flags), resource(resource), StateFrom(StateFrom), StateTo(StateTo) {};
    
    uint8_t flags;// 1 - begin, 2 end, 3 both, 4 - read, 8 - write
    GpuResource::ResourceState StateFrom;
    GpuResource::ResourceState StateTo;
    IRenderDevice::RESOURCEHANDLE resource;
    
    inline bool operator==(const ResourceStateTransition& rhs) const
    {
        return
        rhs.resource == resource
        && (rhs.flags == flags)
        && rhs.StateFrom == StateFrom
        && rhs.StateTo == StateTo;
    }
};

struct SyncThreadBlock {
    
    std::vector<std::tuple<EMachineCommands, void*>> GraphicsNodes;
    std::vector<std::tuple<EMachineCommands, void*>> ComputeNodes;
    std::vector<std::tuple<EMachineCommands, void*>> CopyNodes;
    
    SyncThreadBlock(
        std::shared_ptr<std::vector<ResourceStateTransition>>
        transitionsBegin,
        std::shared_ptr<std::vector<ResourceStateTransition>>
        transitionsEnd
        ) :
    transitionsBegin(transitionsBegin),
    transitionsEnd  (transitionsEnd)
    {}
    
    bool TryAdd(const RenderGraphNode&,
    const ResourceStateTransition transitions[50],
    uint8_t transitionsNum
    );
        
    std::shared_ptr<std::vector<ResourceStateTransition>> transitionsBegin;
    std::shared_ptr<std::vector<ResourceStateTransition>> transitionsEnd;
};


class RenderGraph {

public:
    
    std::vector<SyncThreadBlock> Blocks;
    std::vector<std::pair<
        std::shared_ptr<std::vector<ResourceStateTransition>>,
        std::shared_ptr<std::vector<ResourceStateTransition>>>
        > TransitionPull;
    uint32_t TransitionPullShift  = 0;

    RenderGraph(IRenderDevice* Device) {
        TransitionPull.push_back({});
        Blocks.emplace_back(TransitionPull[0].first, TransitionPull[0].second);
    }
    
    void AddCommand(
    RenderGraphNode Node,
    const std::vector<ResourceStateTransition>& transitionsIn
    );
    
    void Clear();

};



}
