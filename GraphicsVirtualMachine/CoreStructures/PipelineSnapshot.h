#pragma once
#include "BaseStructures.h"
#include "VirtualMachine/ResourceManager/ResourcesManager.h"

namespace GVM {

namespace Compressed {
    struct PipelineSnapshot;
}

struct PipelineSnapshot {

    using CompressedType = Compressed::PipelineSnapshot;
    Shader*                    VS;
    Shader*                    PS;
    Shader*                    CS;
    Shader*                    GS;
    Shader*                    HS;
    Shader*                    DS;

    uint32_t                    DrawCallsNum = 0;

    EPrimitiveTopology          primitiveType = EPrimitiveTopology::PRIMITIVE_TOPOLOGY_UNDEFINED;

    RasterizerStateDesc         rasterizerState = RasterizerStateDesc::Default;
    DepthStencilStateDesc       depthStencilState = DepthStencilStateDesc::Default;
    InputLayout*               vertexDeclaration = nullptr;
    DepthStencilView*          DepthStencilBuffer = nullptr;

    Mesh                        mesh = Mesh::VoidMesh;
    
    CoreBlendDesc               blendDesc = CoreBlendDesc();
    
    uint8_t                     renderTargetsNum = 0;
    RenderTargetDesc            RenderTargets[8] = {};
        
    uint8_t                     samplersNum = 0;
    SamplerStateDesc            Samplers[16] = {};
        
    uint8_t                     viewportsNum = 0;
    ViewportDesc                Viewports[16] = {};
        
        
    uint8_t                     constBuffersNum = 0;
    ConstBuffer*               ConstBuffers[15] = {};
        
    uint8_t                     texturesNum = 0;
    ResourceView*              Textures[128] = {};

    uint32_t GetSize() const;
    void Compress(Compressed::PipelineSnapshot* pointer, const ResourcesManager&) const;
    
};

namespace Compressed {
    
    struct PipelineSnapshot {
        Shader*                    VS;
        Shader*                    PS;
        Shader*                    CS;
        Shader*                    GS;
        Shader*                    HS;
        Shader*                    DS;
        
        uint32_t                    DrawCallsNum;


        //RasterizerStateDesc         rasterizerState;
        uint64_t                    rasterizerState;   
        // DepthStencilStateDesc       depthStencilState;
        uint64_t                    depthStencilState;   
        InputLayout*                vertexDeclaration;
        IndexBufferView*            indexBuffer;

        EPrimitiveTopology          primitiveType;
        
        CoreBlendDesc               blendDesc;
    
        DepthStencilView*           DepthBuffer;
        uint8_t                     renderTargetsNum;
        //RenderTargetDesc            RenderTargets[8];
        
        uint8_t                     samplersNum;
        //SamplerStateDesc            Samplers[16];
        
        uint8_t                     viewportsNum;
        //ViewportDesc                Viewports[16];

        
        uint8_t vertexBuffersNum = 0;
        //IVertexBufferView* vertexBuffers [32];
        //uint32_t vertexStride [32];
        //uint32_t vertexOffset [32];
        
        uint8_t                     constBuffersNum;
        //IConstBuffer*               ConstBuffers[15];
        
        uint8_t                     texturesNum;
        //IResourceView*              Textures[128];

        uint32_t GetSize() const;
        
        uint8_t Data[0];
    
    };

}


}
