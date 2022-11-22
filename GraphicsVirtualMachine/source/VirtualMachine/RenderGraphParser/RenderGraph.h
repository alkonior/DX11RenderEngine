#pragma once
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

struct SyncThreadBlock {
    std::vector<std::tuple<EMachineCommands, void*>> Nodes;

    bool TryAdd(const RenderGraphNode&,
    const std::vector<Resource*>& ReadDependencies,
    const std::vector<Resource*>& WrightDependencies
    );
        
    std::vector<Resource*> ReadDependencies;
    std::vector<Resource*> WrightDependencies;
};


class RenderGraph {
    IRenderDevice* RenderDevice;


public:
    
    std::vector<SyncThreadBlock> Blocks;

    RenderGraph(IRenderDevice* Device): RenderDevice(Device) {
    Blocks.push_back({}); }
    
    void AddCommand(
        RenderGraphNode Node,
        const std::vector<Resource*>& ReadDependencies = {},
        const std::vector<Resource*>& WrightDependencies = {}
    );
    
    void Clear();

};



}
