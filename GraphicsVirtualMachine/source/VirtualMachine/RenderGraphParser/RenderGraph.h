#pragma once
#include "../PipelineSnapshot.h"
#include "IRenderDevice.h"

namespace GVM {



struct RenderGraphNode {
    RenderGraphNode(
    EMachineCommands Command,
    void* Description,
    std::vector<Resource*>&& ReadDependencies,
    std::vector<Resource*>&& WrightDependencies
    );

    
    EMachineCommands Command;
    void* Description;

    std::vector<Resource*> ReadDependencies;
    std::vector<Resource*> WrightDependencies;
};

struct SyncThreadBlock {
    std::vector<RenderGraphNode> Nodes;

    bool TryAdd(const RenderGraphNode& Node);
        
    std::set<Resource*> ReadDependencies;
    std::set<Resource*> WrightDependencies;
};


class RenderGraph {
    IRenderDevice* RenderDevice;

    


    int lasrAddPS = -1;


    
public:
    
    std::vector<SyncThreadBlock> Blocks;

    RenderGraph(IRenderDevice* Device): RenderDevice(Device) { }
    
    void AddCommand(RenderGraphNode Node);
    
    void Clear();

};



}
