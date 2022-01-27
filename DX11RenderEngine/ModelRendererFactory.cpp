#include "ModelRendererFactory.h"


using namespace Renderer;


ShaderDefines ModelRendererDefines[ ] = {
    {"RED", "1"},
    {"FADE", "1"},
    {"TEXTURED", "1"}
};

const D3D11_INPUT_ELEMENT_DESC  ModelVertex::InputElements[ ] =
{
    { "Position",    0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "NORMAL",      0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD",    0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

ModelRendererFactory::ModelRendererFactory(IRenderer* renderer, IStateProvider* provider, void* shaderData, size_t dataSize)
    :PipelineFactory(renderer, provider, ModelRendererDefines, std::size(ModelRendererDefines), shaderData, dataSize, (void*)ModelVertex::InputElements, std::size(ModelVertex::InputElements), D3DCOMPILE_DEBUG) {
}