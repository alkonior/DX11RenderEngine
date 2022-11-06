#include "DebugPassProvider.h"

#include <d3d11.h>
#include <array>

#include "RenderFlags.h"
#include "ResourceManagers/States/BlendStates.h"
#include "ResourceManagers/States/DSStates.h"
#include "ResourceManagers/States/RasterizerStates.h"
#include "Utils/DrawData.h"
void DebugPassProvider::PatchPipelineState(Renderer::Pipeline* refToPS, uint32_t pipelineFlags)
{
    DebugPassFlags flags;
    flags.flags = pipelineFlags;
    refToPS->bs = &BlendStates::NoAlpha;

    refToPS->dss = &DepthStencilStates::NoDSS;

    refToPS->rs = &RasterizerStates::CClockWise;

}

const D3D11_INPUT_ELEMENT_DESC  D3DInputElements[] =
{
    { "Position",    0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

const D3D11_INPUT_ELEMENT_DESC  D2DInputElements[] =
{
    { "Position",    0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};



Renderer::InputLayoutDescription DebugPassProvider::GetInputLayoutDescription(uint32_t definesFlags)
{
    if (definesFlags & D2D)
        return Renderer::InputLayoutDescription{ (void*)D2DInputElements, std::size(D2DInputElements) };

    return Renderer::InputLayoutDescription{ (void*)D3DInputElements, std::size(D3DInputElements) };
}


const Renderer::ShaderDefines ModelRendererDefines[] = {
    {"RED", "1"},
    {"D2D", "1"},
    {"D3D", "1"},
};


Renderer::PipelineFactoryDescription DebugPassProvider::GetFactoryDescription()
{
    return {
        ModelRendererDefines,
        std::size(ModelRendererDefines)
    };
}
const char* DebugPassProvider::GetShaderName()
{
    return "DebugShader";
}
DebugPassProvider::~DebugPassProvider()
{
    
}
