#pragma once

#include "PipelineFactory.h"
#include "SimpleMath.h"
#include "ModelRendererUtils.h"
#include "Transform.h"


enum ModelDefines{
    ZERO = 0,
    RED = 1,
    LERP = 2
};

struct Shader3DCosntBuffer {
    matrix world;
    matrix view;
    matrix projection;
};


class ModelRendererFactory: public Renderer::PipelineFactory {
public:
    ModelRendererFactory(Renderer::IRenderer* renderer, Renderer::IStateProvider* provider, void* shaderData, size_t dataSize);
    static const Renderer::ShaderDefines ModelRendererDefines[];
};

