#pragma once

#include "PipelineFactory.h"
#include "SimpleMath.h"
#include "ModelRendererUtils.h"
#include "Transform.h"


enum ModelDefines{
    ZERO = 0,
    RED = 1,
    LERP = 2,
    SINGLE_FRAME = 4,
    BAD_UV = 8,
};


#pragma pack(push, 4)
struct Shader3DTranformCosntBuffer {
    matrix world;
    matrix view;
    matrix projection;
};

struct Shader3DDataCosntBuffer {
    float alpha;
    float w;
    float h;
};
#pragma pack(pop)

class ModelRendererFactory: public Renderer::PipelineFactory {
public:
    ModelRendererFactory(Renderer::IRenderer* renderer, Renderer::IStateProvider* provider, void* shaderData, size_t dataSize);
    static const Renderer::ShaderDefines ModelRendererDefines[];
};

