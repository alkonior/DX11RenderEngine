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

namespace GVM {

struct FColor {
    float Color[4];
};

struct UBox {
    uint32_t left;
    uint32_t top;
    uint32_t front;
    uint32_t right;
    uint32_t bottom;
    uint32_t back;
};

struct FBox {
    float left;
    float top;
    float front;
    float right;
    float bottom;
    float back;
};

struct Rect {
    int64_t left;
    int64_t top;
    int64_t right;
    int64_t bottom;
};

struct FRect {
    float Color[4];
};

}

