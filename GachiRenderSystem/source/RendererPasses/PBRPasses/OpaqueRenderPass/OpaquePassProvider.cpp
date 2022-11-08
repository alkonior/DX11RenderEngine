#include "OpaquePassProvider.h"

#include <d3d11.h>
#include <array>

#include "RenderFlags.h"
#include "ResourceManagers/States/BlendStates.h"
#include "ResourceManagers/States/DSStates.h"
#include "ResourceManagers/States/RasterizerStates.h"
#include "Utils/DrawData.h"
void OpaquePassProvider::PatchPipelineState(Renderer::Pipeline* refToPS, uint32_t pipelineFlags)
{
    ModelsFlags flags;
    flags.flags = pipelineFlags;
    refToPS->bs = &BlendStates::Alpha;

    refToPS->dss = &DepthStencilStates::DSS;

    refToPS->rs = &RasterizerStates::CClockWise;

    if (flags.isWireframe)
    {
        refToPS->rs = &RasterizerStates::Wireframe;
    }
}

const D3D11_INPUT_ELEMENT_DESC  DefaultInputElements[] =
{
    { "POSITION",      0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "NORMAL"  ,      0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "BINORMAL",      0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TANGENT" ,      0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "COLOR"   ,      0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD",      0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};


Renderer::InputLayoutDescription OpaquePassProvider::GetInputLayoutDescription(uint32_t definesFlags)
{
        return Renderer::InputLayoutDescription{ (void*)DefaultInputElements, std::size(DefaultInputElements) };
}


const Renderer::ShaderDefines ModelRendererDefines[] = {
    {"RED", "1"},
};


Renderer::PipelineFactoryDescription OpaquePassProvider::GetFactoryDescription()
{
    return {
        ModelRendererDefines,
        std::size(ModelRendererDefines)
    };
}
const char* OpaquePassProvider::GetShaderName()
{
    return "OpaqueShader";
}
OpaquePassProvider::~OpaquePassProvider()
{
    
}
