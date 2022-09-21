#pragma once
#include "GVMBaseStructures\VertexDeclaration.h"
#include "GVMBaseStructures\Shader.h"
#include "GVMBaseStructures\PipelineStates\BlendState.h"
#include "GVMBaseStructures\PipelineStates\RasterizerState.h"
#include "GVMBaseStructures\PipelineStates\SamplerState.h"
#include "GVMBaseStructures\PipelineStates\DepthStencilState.h"
#include "GVMBaseStructures\PipelineStates\Viewport.h"
#include "GVMBaseStructures\IRenderClasses.h"
#include "GVMBaseStructures\ResourceViews\BaseViews.h"
#include "GVMBaseStructures\ResourceViews\ShaderResourceViewDesc.h"
#include "GVMBaseStructures\ResourceViews\ResourceDesc.h"


namespace GVM {

struct FColor {
    FColor() = default;
    FColor(float Colors[4]):Color{Colors[0],Colors[1],Colors[2],Colors[3]}{}
    FColor(float r,float g,float b,float a):Color{r,g,b,a}{}
   // FColor(uint8_t r,uint8_t g,uint8_t b,uint8_t a):Color{r*1.f/255,g*1.f/255,b*1.f/255,a*1.f/255}{}
    
    float Color[4] = {0,0,0,0};
};

struct UBox {
    uint32_t Left;
    uint32_t Top;
    uint32_t Front;
    uint32_t Right;
    uint32_t Bottom;
    uint32_t Back;
};

struct FBox {
    float Left;
    float Top;
    float Front;
    float Right;
    float Bottom;
    float Back;
};

struct Rect {
    int64_t Left;
    int64_t Top;
    int64_t Right;
    int64_t Bottom;
};

struct FRect {
    float Color[4];
};

struct FDrawCall {
    EDrawCallType Type;
    uint32_t Args[5];
};

struct DrawCall
{
    DrawCall(EDrawCallType type, std::tuple<uint32_t, uint32_t, uint32_t, uint32_t, uint32_t> list): type(type), args(list)
    {}
    EDrawCallType type;
    std::tuple<uint32_t, uint32_t, uint32_t, uint32_t, uint32_t> args;

    template<size_t index>
    uint32_t get()
    {
        return std::get<index>(args);
    }
};




namespace Compressed {
struct VertexBufferBinding {
    //VertexBufferView* vertexBuffers [1];
    //  uint32_t vertexStride [1];
    //  uint32_t vertexOffset [1];
};
}

struct VertexBufferBinding {
    //using CompressedType = Compressed::VertexBufferBinding;
    uint8_t buffersNum = 0;
    VertexBufferView* vertexBuffers [32] = {};
};

namespace Compressed {
struct RenderTargetDesc {
    RenderTargetView* rtv = nullptr;
    BlendStateDesc BlendState;
    
};
}

struct RenderTargetDesc {
    using CompressedType = Compressed::RenderTargetDesc;
    RenderTargetView* rtv = nullptr;
    BlendStateDesc BlendState;
    operator CompressedType() const { return {rtv,BlendState}; }
};

struct Mesh  {
    //static const Mesh VoidMesh;
    VertexBufferBinding vertexBuffer;
    IndexBufferView* indexBuffer = nullptr;
};

}

