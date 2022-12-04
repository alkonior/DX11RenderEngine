#include "PostProcessRenderPassProvider.h"

#include <d3d11.h>
#include <xutility>

#include "ResourceManagers/States/BlendStates.h"
#include "ResourceManagers/States/DSStates.h"
#include "ResourceManagers/States/RasterizerStates.h"



void PostProcessRenderPassProvider::PatchPipelineState(Renderer::Pipeline* refToPS, uint32_t definesFlags)
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

Renderer::InputLayoutDescription PostProcessRenderPassProvider::GetInputLayoutDescription(uint32_t definesFlags)
{
    return {(void*)DefaultInputElements, 2};
}

const Renderer::ShaderDefines PostProcessRendererDefines[] = {
    Renderer::ShaderDefines("RED"),
    Renderer::ShaderDefines("CopyColor"),
    Renderer::ShaderDefines("Simple"),
    Renderer::ShaderDefines("WithLum"),
    Renderer::ShaderDefines("WithLumEx")
};

Renderer::PipelineFactoryDescription PostProcessRenderPassProvider::GetFactoryDescription()
{
    return {
        PostProcessRendererDefines,
        std::size(PostProcessRendererDefines),
        Renderer::UseComputeShader,
        1
    };
    
}
const char* PostProcessRenderPassProvider::GetShaderName()
{
    return "TAAShader";
}
