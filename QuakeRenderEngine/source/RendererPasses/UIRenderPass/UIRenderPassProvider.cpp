#include "UIRenderPassProvider.h"

#include <d3d11.h>
#include <array>

#include "ResourceManagers/States/BlendStates.h"
#include "ResourceManagers/States/DSStates.h"
#include "ResourceManagers/States/RasterizerStates.h"

const Renderer::ShaderDefines UIRendererDefines[] = {
    {"RED","1"},
    {"COLORED","1"},
    {"SCALED","1"},
};


void UIRenderPassProvider::PatchPipelineState(Renderer::Pipeline* refToPS, uint32_t definesFlags)
{
    refToPS->bs = &BlendStates::Alpha;
    refToPS->dss = &DepthStencilStates::NoDSS;
    refToPS->rs = &RasterizerStates::CClockWise;
}


const D3D11_INPUT_ELEMENT_DESC UIInputElements[] =
{
    {"Position",0,DXGI_FORMAT_R32G32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
    {"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0, D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
};

Renderer::InputLayoutDescription UIRenderPassProvider::GetInputLayoutDescription(uint32_t definesFlags)
{
    return Renderer::InputLayoutDescription{(void*)UIInputElements,std::size(UIInputElements)};
}
Renderer::PipelineFactoryDescription UIRenderPassProvider::GetFactoryDescription()
{
    uint16_t debug = 0;
#ifdef _DEBUG
    debug = 1;
#endif
    
    return {UIRendererDefines,sizeof(UIRendererDefines),debug};
}


const char* UIRenderPassProvider::GetShaderName()
{
    return "UI shader";
}
