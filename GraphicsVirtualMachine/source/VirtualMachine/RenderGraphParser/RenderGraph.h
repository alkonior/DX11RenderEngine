#pragma once
#include "../PipelineSnapshot.h"
#include "IRenderDevice.h"

namespace GVM {




class RenderGraph {
    IRenderDevice* Device;

    struct RenderGraphNode {
        enum Type { };
        union {
            struct PipelineSnapshotNode {
                Compressed::PipelineSnapshot* ps;
            } PSNode;

            struct SetResourceNode {
                SetResourceDataDesc* srd;
            } SRNode;

            struct ClearRenderTargetNode {
                ClearRenderTargetDesc* crt;
            } CRTNode;

            struct ClearDepthStencilNode {
                ClearDepthStencilDesc* cds;
            } CDSNode;
        };

        RenderGraphNode(Compressed::PipelineSnapshot* ps);
        RenderGraphNode(SetResourceDataDesc* srd);
        RenderGraphNode(ClearRenderTargetDesc* crt);
        RenderGraphNode(ClearDepthStencilDesc* cds);

        Resource** ReadDependencies;
        Resource** WrightDependencies;
    };

    struct SyncReadBlock {
        std::set<Resource*> ReadDependencies;
        
    };

    struct SyncThreadBlock {
        std::set<Resource*> WrightDependencies;
        std::vector<RenderGraphNode> Nodes;
    };
public:

    RenderGraph(IRenderDevice* Device):Device(Device) {}
    
    void AddPipelineSnapshotNode(Compressed::PipelineSnapshot* ps);
    void AddSetResourceNode(SetResourceDataDesc* srd);
    void AddClearRenderTargetNode(ClearRenderTargetDesc* crt);
    void AddClearDepthStencilNode(ClearDepthStencilDesc* cds);
    //void  AddClearPipelineNode   (Compressed::PipelineSnapshot* ps);


    void Clear();

};



}
