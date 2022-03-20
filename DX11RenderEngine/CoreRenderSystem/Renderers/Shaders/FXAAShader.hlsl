#define HLSL
#include "P:\Quake-2\ref_dx11rg\DX11RenderEngine\DX11RenderEngine\CoreRenderSystem/CoreShaderInclude.h"
#include "P:\Quake-2\ref_dx11rg\DX11RenderEngine\DX11RenderEngine\CoreRenderSystem/Renderers/FXAARenderer/FXAARendererConstBuffer.h"

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



#ifndef SETLUMA

#define FXAA_PC 1
#define FXAA_HLSL_4 1
#define FXAA_QUALITY__PRESET 12
#define FXAA_GREEN_AS_LUMA 0

#include "P:\Quake-2\ref_dx11rg\DX11RenderEngine\DX11RenderEngine\CoreRenderSystem/Renderers/FXAARenderer/Fxaa3_11.h"

Texture2D diffuseColor : register(t0);
SamplerState basicSampler : register(s0);

float4 psIn(PSIn input) : SV_Target
{
    float4 unused = float4(0,0,0,0);

    FxaaFloat2 pos = input.uv;
    FxaaFloat4 fxaaConsolePosPos = unused;
    FxaaTex tex = { basicSampler, diffuseColor };
    FxaaTex fxaaConsole360TexExpBiasNegOne = { basicSampler, diffuseColor };
    FxaaTex fxaaConsole360TexExpBiasNegTwo = { basicSampler, diffuseColor };

    float w,h;
    diffuseColor.GetDimensions(w,h);

    FxaaFloat2 fxaaQualityRcpFrame = float2(1.0/w, 1.0/h);

    FxaaFloat4 fxaaConsoleRcpFrameOpt = unused;
    FxaaFloat4 fxaaConsoleRcpFrameOpt2 = unused;
    FxaaFloat4 fxaaConsole360RcpFrameOpt2 = unused;
    FxaaFloat fxaaQualitySubpix =              FXAABuffer.fxaaQualitySubpix;
    FxaaFloat fxaaQualityEdgeThreshold =       FXAABuffer.fxaaQualityEdgeThreshold;
    FxaaFloat fxaaQualityEdgeThresholdMin =    FXAABuffer.fxaaQualityEdgeThresholdMin;
    FxaaFloat fxaaConsoleEdgeSharpness =       FXAABuffer.fxaaConsoleEdgeSharpness;
    FxaaFloat fxaaConsoleEdgeThreshold =       FXAABuffer.fxaaConsoleEdgeThreshold;
    FxaaFloat fxaaConsoleEdgeThresholdMin =    FXAABuffer.fxaaConsoleEdgeThresholdMin;
    FxaaFloat4 fxaaConsole360ConstDir = unused;

    return float4(FxaaPixelShader(
        pos,
        fxaaConsolePosPos,
        tex,
        fxaaConsole360TexExpBiasNegOne,
        fxaaConsole360TexExpBiasNegTwo,
        fxaaQualityRcpFrame,
        fxaaConsoleRcpFrameOpt,
        fxaaConsoleRcpFrameOpt2,
        fxaaConsole360RcpFrameOpt2,
        fxaaQualitySubpix,
        fxaaQualityEdgeThreshold,
        fxaaQualityEdgeThresholdMin,
        fxaaConsoleEdgeSharpness,
        fxaaConsoleEdgeThreshold,
        fxaaConsoleEdgeThresholdMin,
        fxaaConsole360ConstDir
    ).rgb, 1.0);

}
#else 

Texture2D diffuseColor : register(t0);
SamplerState basicSampler : register(s0);

float CalcLuminance(float3 color)
{
    return max(dot(color, float3(0.299f, 0.587f, 0.114f)), 0.0001f);
}

float4 psIn(PSIn input) : SV_Target{
    float4 color = diffuseColor.Sample(basicSampler, input.uv);
    return float4(color.rgb, CalcLuminance(color.rgb));
}
#endif