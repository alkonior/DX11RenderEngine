#pragma once

#include "PipelineFactory.h"
#include "SimpleMath.h"
#include "ModelRendererUtils.h"
#include "TransformUtils.h"


enum ModelDefines{
    MZERO = 0,
    MRED = 1,
    MLERP = 2,
    MSINGLE_FRAME = 4,
    MBAD_UV = 8,
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

