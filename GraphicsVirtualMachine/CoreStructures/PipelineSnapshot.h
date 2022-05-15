#pragma once
#include "BaseStructures.h"

namespace GVM {

namespace Compressed {
    struct PipelineSnapshot;
}

struct PipelineSnapshot {

    using CompressedType = Compressed::PipelineSnapshot;
    IShader*                    VS;
    IShader*                    PS;
    IShader*                    CS;
    IShader*                    GS;
    IShader*                    HS;
    IShader*                    DS;

    uint32_t                    DrawCallsNum;

    EPrimitiveTopology          primitiveType;

    RasterizerStateDesc         rasterizerState;
    DepthStencilStateDesc       depthStencilState;
    IInputLayout*               vertexDeclaration;
    IDepthStencilView*          DepthBuffer;

    Mesh                        mesh;

    
    uint8_t                     renderTargetsNum;
    RenderTargetDesc            RenderTargets[8];
        
    uint8_t                     samplersNum;
    SamplerStateDesc            Samplers[16];
        
    uint8_t                     viewportsNum;
    ViewportDesc                Viewports[16];
        
        
    uint8_t                     constBuffersNum;
    IConstBuffer*               ConstBuffers[15];
        
    uint8_t                     texturesNum;
    IResourceView*              Textures[128];

    uint32_t GetSize();
    void Compress(void* pointer);
    
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

        uint8_t Data[0];
    
    };

}


}
