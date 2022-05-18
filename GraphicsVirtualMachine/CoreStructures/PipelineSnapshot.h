#pragma once
#include "BaseStructures.h"
#include "VirtualMachine/ResourcesManager.h"

namespace GVM {

namespace Compressed {
    struct PipelineSnapshot;
}

struct PipelineSnapshot {

    using CompressedType = Compressed::PipelineSnapshot;
    IShader*                    VS = nullptr;
    IShader*                    PS = nullptr;
    IShader*                    CS = nullptr;
    IShader*                    GS = nullptr;
    IShader*                    HS = nullptr;
    IShader*                    DS = nullptr;

    uint32_t                    DrawCallsNum = 0;

    EPrimitiveTopology          primitiveType = EPrimitiveTopology::PRIMITIVE_TOPOLOGY_UNDEFINED;

    RasterizerStateDesc         rasterizerState = RasterizerStateDesc::Default;
    DepthStencilStateDesc       depthStencilState = DepthStencilStateDesc::Default;
    IInputLayout*               vertexDeclaration = nullptr;
    IDepthStencilView*          DepthStencilBuffer = nullptr;

    Mesh                        mesh = Mesh::VoidMesh;
    
    CoreBlendDesc               blendDesc = CoreBlendDesc();
    
    uint8_t                     renderTargetsNum = 0;
    RenderTargetDesc            RenderTargets[8] = {};
        
    uint8_t                     samplersNum = 0;
    SamplerStateDesc            Samplers[16] = {};
        
    uint8_t                     viewportsNum = 0;
    ViewportDesc                Viewports[16] = {};
        
        
    uint8_t                     constBuffersNum = 0;
    IConstBuffer*               ConstBuffers[15] = {};
        
    uint8_t                     texturesNum = 0;
    IResourceView*              Textures[128] = {};

    uint32_t GetSize() const;
    void Compress(Compressed::PipelineSnapshot* pointer, const ResourcesManager&) const;
    
};

namespace Compressed {
    
    struct PipelineSnapshot {
        IShader*                    VS;
        IShader*                    PS;
        IShader*                    CS;
        IShader*                    GS;
        IShader*                    HS;
        IShader*                    DS;
        
        uint32_t                    DrawCallsNum;


        //RasterizerStateDesc         rasterizerState;
        uint64_t                    rasterizerState;   
        // DepthStencilStateDesc       depthStencilState;
        uint64_t                    depthStencilState;   
        IInputLayout*               vertexDeclaration;
        IDepthStencilView*          DepthBuffer;
        IIndexBufferView*           indexBuffer;

        EPrimitiveTopology          primitiveType;
        
        CoreBlendDesc               blendDesc;
    
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
