#include "TAARenderPassProvider.h"

#include <d3d11.h>

#include "ResourceManagers/States/BlendStates.h"
#include "ResourceManagers/States/DSStates.h"
#include "ResourceManagers/States/RasterizerStates.h"



void TAARenderPassProvider::PatchPipelineState(Renderer::Pipeline* refToPS, uint32_t definesFlags)
{
    refToPS->bs = &BlendStates::NoAlpha;
    refToPS->dss = &DepthStencilStates::NoDSS;
    refToPS->rs = &RasterizerStates::All;
}
const D3D11_INPUT_ELEMENT_DESC DefaultInputElements[] =
{
    {"Position",  0, DXGI_FORMAT_R32G32_FLOAT,  0,                            0, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
};

Renderer::InputLayoutDescription TAARenderPassProvider::GetInputLayoutDescription(uint32_t definesFlags)
{
    return {(void*)DefaultInputElements, 2};
}

const Renderer::ShaderDefines TAARendererDefines[] = {
    Renderer::ShaderDefines("RED"),
    Renderer::ShaderDefines("COPY"),
    Renderer::ShaderDefines("USEYUV"),
};

Renderer::PipelineFactoryDescription TAARenderPassProvider::GetFactoryDescription()
{
    return {
        TAARendererDefines,
        sizeof(TAARendererDefines),
        Renderer::UseComputeShader,
        1
    };
    
}
const char* TAARenderPassProvider::GetShaderName()
{
    return "TAAShader";
}
