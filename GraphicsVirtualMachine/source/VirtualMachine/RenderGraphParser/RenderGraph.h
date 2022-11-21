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
        
    std::set<Resource*> ReadDependencies;
    std::set<Resource*> WrightDependencies;
    std::set<Resource*> ForwardReadDependencies;
    std::set<Resource*> ForwardWrightDependencies;
};


class RenderGraph {
    IRenderDevice* RenderDevice;

    


    int lasrAddPS = -1;
    int lasrLockedBlock = 0;


    
public:
    
    std::vector<SyncThreadBlock> Blocks;

    RenderGraph(IRenderDevice* Device): RenderDevice(Device) { }
    
    void AddCommand(
        RenderGraphNode Node,
        const std::vector<Resource*>& ReadDependencies,
        const std::vector<Resource*>& WrightDependencies
    );
    
    void Clear();

};



}
