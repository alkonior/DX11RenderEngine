#include "ModelsPassProvider.h"

#include <d3d11.h>
#include <array>

#include "RenderFlags.h"
#include "ResourceManagers/States/BlendStates.h"
#include "ResourceManagers/States/DSStates.h"
#include "ResourceManagers/States/RasterizerStates.h"
#include "Utils/DrawData.h"
void ModelsPassProvider::PatchPipelineState(Renderer::Pipeline* refToPS, uint32_t pipelineFlags)
{
    ModelsFlags flags;
    flags.flags = pipelineFlags;
    refToPS->bs = &BlendStates::NoAlpha;

    refToPS->dss = &DepthStencilStates::DSS;

    refToPS->rs = &RasterizerStates::CClockWise;

    if (flags.isWireframe)
    {
        refToPS->rs = &RasterizerStates::Wireframe;
    }
}

const D3D11_INPUT_ELEMENT_DESC  DefaultInputElements[] =
{
    { "Position",    0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "NORMAL",      0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD",    0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

const D3D11_INPUT_ELEMENT_DESC  LerpInputElements[] =
{
    { "TEXCOORD",      0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "Position",      1, DXGI_FORMAT_R32G32B32_FLOAT,    1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "NORMAL",        1, DXGI_FORMAT_R32G32B32_FLOAT,    1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "Position",      2, DXGI_FORMAT_R32G32B32_FLOAT,    2, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "NORMAL",        2, DXGI_FORMAT_R32G32B32_FLOAT,    2, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};


const D3D11_INPUT_ELEMENT_DESC  SingleFrameInputElements[] =
{
    { "Position",           0, DXGI_FORMAT_R32G32B32_FLOAT,    1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "NORMAL",             0, DXGI_FORMAT_R32G32B32_FLOAT,    1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD",           0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

Renderer::InputLayoutDescription ModelsPassProvider::GetInputLayoutDescription(uint32_t definesFlags)
{
        return Renderer::InputLayoutDescription{ (void*)DefaultInputElements, std::size(DefaultInputElements) };
}


const Renderer::ShaderDefines ModelRendererDefines[] = {
    {"RED", "1"},
    {"LERP", "1"},
    {"BAD_UV", "1"},
    {"MNONORMAL", "1"},
    {"COLORED", "1"},
    {"LIGHTED", "1"},
};


Renderer::PipelineFactoryDescription ModelsPassProvider::GetFactoryDescription()
{
    return {
        ModelRendererDefines,
        std::size(ModelRendererDefines)
    };
}
const char* ModelsPassProvider::GetShaderName()
{
    return "ModelsShader";
}
ModelsPassProvider::~ModelsPassProvider()
{
    
}
