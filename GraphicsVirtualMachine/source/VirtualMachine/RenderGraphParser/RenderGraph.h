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


struct TryAddResult {
    std::vector<ResourceStateTransition> NewBeginTrans;
    std::vector<ResourceStateTransition> NewEndTrans;
    bool forceAdd = false; 
    TryAddResult& operator=(TryAddResult&& rhs) noexcept
    {
        NewBeginTrans = std::move(rhs.NewBeginTrans);
        NewEndTrans = std::move(rhs.NewEndTrans);
        rhs.NewBeginTrans.clear();
        rhs.NewEndTrans.clear();
        return *this;
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
    {
        transitionsBegin->clear();
        transitionsEnd->clear();
    }
    
    bool TryAdd(
    const ResourceStateTransition transitionsIn[50],
    uint8_t transitionsNum,
    TryAddResult& result
    );
    
    void Add(
    const RenderGraphNode&,
    const TryAddResult& result
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
        > TransitionsPull;
    uint32_t TransitionPullShift  = 0;

    const uint8_t MaxIterationToAdd = 2;;
    const bool enabled = false;

    RenderGraph(IRenderDevice* Device) {
        TransitionsPull.push_back({});
        TransitionsPull[0].first  = std::make_shared<std::vector<ResourceStateTransition>>();
        TransitionsPull[0].second  = std::make_shared<std::vector<ResourceStateTransition>>();
        Blocks.emplace_back(TransitionsPull[0].first, TransitionsPull[0].second);
    }

    void ResizeTransitionsPull(size_t size)
    {
        if (TransitionsPull.size() < size)
        {
            TransitionsPull.resize(size);
            for (auto& tp : TransitionsPull)
            {
                if (tp.first == nullptr)
                {
                    tp.first  = std::make_shared<std::vector<ResourceStateTransition>>();
                    tp.second  = std::make_shared<std::vector<ResourceStateTransition>>();
                }
            }
        }
    }
    
    void AddCommand(
    RenderGraphNode Node,
    const std::vector<ResourceStateTransition>& transitionsIn
    );
    
    void Clear();

};



}
