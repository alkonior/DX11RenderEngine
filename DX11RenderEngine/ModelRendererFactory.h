#pragma once

#include "PipelineFactory.h"
#include "SimpleMath.h"
#include "ModelRendererUtils.h"
#include "Transform.h"

extern Renderer::ShaderDefines ModelRendererDefines[ ];

struct Shader3DCosntBuffer {
    matrix world;
    matrix view;
    matrix projection;
};


class ModelRendererFactory: public Renderer::PipelineFactory {
public:
    ModelRendererFactory(Renderer::IRenderer* renderer, Renderer::IStateProvider* provider, void* shaderData, size_t dataSize);

};

