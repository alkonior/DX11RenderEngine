#define HLSL
#include "P:\Quake-2\ref_dx11rg\DX11RenderEngine\DX11RenderEngine\CoreRenderSystem/CoreShaderInclude.h"
#include "P:\Quake-2\ref_dx11rg\DX11RenderEngine\DX11RenderEngine\CoreRenderSystem/Renderers/TAARenderer\TAARendererConstBuffer.h"

struct VSIn
{
    float2 pos : Position;
    float2 uv : TEXCOORD;
};

struct PSIn
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
};


PSIn vsIn(VSIn input)
{
    PSIn output = (PSIn)0;

    output.pos = float4(input.pos, 0.0f, 1.0f);
    output.uv = input.uv;
    return output;
}


#ifndef COPY

Texture2D curColor  : register(t0);
Texture2D curDepth  : register(t1);
Texture2D pastColor : register(t2);
Texture2D pastDepth : register(t3);
Texture2D velocityField  : register(t4);

float3 rgb_to_ycbcr(float3 rgb) {
    float y = 0.299 * rgb.x + 0.587 * rgb.y + 0.114 * rgb.z;
    float cb = (rgb.z - y) * 0.565;
    float cr = (rgb.x - y) * 0.713;

    return float3(y, cb, cr);
}

float3 ycbcr_to_rgb(float3 yuv) {
    return float3(
        yuv.x + 1.403 * yuv.z,
        yuv.x - 0.344 * yuv.y - 0.714 * yuv.z,
        yuv.x + 1.770 * yuv.y
    );
}

SamplerState basicSampler : register(s0);

float4 lerp(float alpha, float4 left, float4 right)
{
    return left*(1-alpha)+alpha*right;
}

float4 psIn(PSIn input) : SV_Target
{
    float2 texCords = input.uv;
    float4 color = curColor.Sample(basicSampler, texCords);
    float2 velocity = velocityField.Sample(basicSampler, texCords);
    float4 pColor = pastColor.Sample(basicSampler, texCords - velocity);

    float depth  = curDepth.Sample(basicSampler, texCords);
    float pDepth = pastDepth.Sample(basicSampler, texCords - velocity);
    
    float alpha = 1.0 -  1.0 / TAABuffer.numSamples;
    
    if (abs(depth-pDepth) > TAABuffer.depthThreshold)
        alpha = 0;

  return lerp(alpha, color, pColor);
}

#else

Texture2D curColor : register(t0);
Texture2D curDepth : register(t1);

SamplerState basicSampler : register(s0);

struct PSO
{
    float4 newColor       : SV_Target0;
    float4 historyColor   : SV_Target1;
    float historyDepth   : SV_Target2;
};


PSO psIn(PSIn input) : SV_Target
{

    PSO pso = (PSO)0;

    float2 texCords = input.uv;
    pso.historyColor = curColor.Sample(basicSampler, texCords);
    pso.newColor = pso.historyColor;
    pso.historyDepth = curDepth.Sample(basicSampler, texCords);

    return  pso;
}

#endif
