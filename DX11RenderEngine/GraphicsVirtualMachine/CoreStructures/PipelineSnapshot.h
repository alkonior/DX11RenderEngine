#pragma once
#include "../BaseStructures.h"
#include "../VirtualMachine/ResourceManager/ResourcesManager.h"

namespace GVM {

    
    
namespace Compressed {
    struct PipelineSnapshot;
}

struct PipelineSnapshot {

    using CompressedType = Compressed::PipelineSnapshot;
    Shader*                     VS = nullptr;
    Shader*                     PS = nullptr;
    Shader*                     CS = nullptr;
    Shader*                     GS = nullptr;
    Shader*                     HS = nullptr;
    Shader*                     DS = nullptr;
    
    uint32_t                    DrawCallsNum = 0;

    EPrimitiveTopology          primitiveType = EPrimitiveTopology::PRIMITIVE_TOPOLOGY_UNKNOWN;

    RasterizerStateDesc         rasterizerState = RasterizerStateDesc::Default;
    DepthStencilStateDesc       depthStencilState = DepthStencilStateDesc::Default;
    InputLayout*                InputDeclaration = nullptr;
    DepthStencilView*           DepthStencilBuffer = nullptr;

    Mesh                        mesh = {};
    
    CoreBlendDesc               blendDesc = CoreBlendDesc();
    
    uint8_t                     renderTargetsNum = 0;
    RenderTargetDesc            RenderTargets[8] = {};
        
    uint8_t                     samplersNum = 0;
    SamplerStateDesc            Samplers[16] = {};
        
    uint8_t                     viewportsNum = 0;
    ViewportDesc                Viewports[16] = {};
        
        
    uint8_t                     constBuffersNum = 0;
    ConstBufferView*            ConstBuffers[15] = {};
        
    uint8_t                     texturesNum = 0;
    ResourceView*               Textures[128] = {};

    uint32_t GetSize(const IStructuresSize& structuresSizes) const;

    struct CompressArgs
    {
        CompressedType* cps;
        ResourcesManager& resourceManager;
        const IStructuresSize& structuresSizes;
    };
    void Compress(const CompressArgs& args) const;
    
};

namespace Compressed {
    
    struct PipelineSnapshot {
        
        uint32_t                    SnapshotByteSize;
        uint32_t                    DrawCallsNum;


        RasterizerStateDesc         rasterizerState;   
        DepthStencilStateDesc       depthStencilState;   

        EPrimitiveTopology          primitiveType;
        
        CoreBlendDesc               blendDesc;
    
        uint8_t                     renderTargetsNum;
        //RenderTargetDesc*           RenderTargets;
        
        uint8_t                     samplersNum;
        //SamplerStateDesc            Samplers[16];
        
        uint8_t                     viewportsNum;
        //ViewportDesc                Viewports[16];

        
        uint8_t                     vertexBuffersNum = 0;
        //IVertexBufferView* vertexBuffers [32];
        
        uint8_t                     constBuffersNum;
        //IConstBuffer*               ConstBuffers[15];
        
        uint8_t                     texturesNum;
        //IResourceView*              Textures[128];


        Shader*                     VS;
        Shader*                     PS;
        Shader*                     CS;
        Shader*                     GS;
        Shader*                     HS;
        Shader*                     DS;
        
        InputLayout*                vertexDeclaration;
        DepthStencilView*           DepthBuffer;
        IndexBufferView*            indexBuffer;

        RenderTargetDesc*           RenderTargets = {};
        SamplerStateDesc*           Samplers = {};
        ViewportDesc*               Viewports = {};
        ConstBufferView*            ConstBuffers = {};
        ResourceView*               Textures = {};
        
        uint8_t Data[];

        
        
    
    };

}


}
