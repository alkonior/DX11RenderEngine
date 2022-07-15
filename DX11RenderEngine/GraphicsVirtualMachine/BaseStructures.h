#pragma once
#include "BaseStructures\VertexDeclaration.h"
#include "BaseStructures\Shader.h"
#include "BaseStructures\PipelineStates\BlendState.h"
#include "BaseStructures\PipelineStates\RasterizerState.h"
#include "BaseStructures\PipelineStates\SamplerState.h"
#include "BaseStructures\PipelineStates\DepthStencilState.h"
#include "BaseStructures\PipelineStates\Viewport.h"
#include "BaseStructures\IRenderClasses.h"
#include "BaseStructures\ResourceViews\BaseViews.h"
#include "BaseStructures\ResourceViews\ShaderResourceViewDesc.h"
#include "BaseStructures\ResourceViews\ResourceDesc.h"


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
        return std::tuple_element<index, decltype(args)>(args);
    }
};


struct IStructuresSize
{
    uint32_t IResourceSize = 0;
    uint32_t IResourceViewSize = 0;
    uint32_t IShaderSize = 0;
    uint32_t IInputLayoutSize = 0;
};
    
}

