#pragma once

#include "BaseStructures\VertexDeclaration.h"
#include "BaseStructures\Resources.h"
#include "BaseStructures\Shader.h"
#include "BaseStructures\RenderDeviceInitParams.h"
#include "BaseStructures\PipelineStates\BlendState.h"
#include "BaseStructures\PipelineStates\RasterizerState.h"
#include "BaseStructures\PipelineStates\SamplerState.h"
#include "BaseStructures\PipelineStates\DepthStencilState.h"
#include "BaseStructures\PipelineStates\Viewport.h"
#include "BaseStructures\PipelineStates\RenderTarget.h"
#include "BaseStructures\IRenderClasses.h"
#include "BaseStructures\ResourceViews\BaseViews.h"
#include "BaseStructures\ResourceViews\ShaderResourceViewDesc.h"


namespace GVM {

struct FColor {
    float Color[4];
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

}

