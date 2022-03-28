#define HLSL
#include "P:\Quake-2\ref_dx11rg\DX11RenderEngine\DX11RenderEngine\CoreRenderSystem/CoreShaderInclude.h"
#include "P:\Quake-2\ref_dx11rg\DX11RenderEngine\DX11RenderEngine\CoreRenderSystem/Renderers/MotionBlurRenderer\MotionBlurConstBuffer.h"

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

//#define DYNAMIC

Texture2D diffuseColor : register(t0);
Texture2D lightColor : register(t1);

#ifndef DYNAMIC
Texture2D<float> depthTexture : register(t2);
Texture2D bloomMask : register(t3);
#endif
#ifdef DYNAMIC
Texture2D<float2> velocityTexture : register(t2);
Texture2D blurStrength : register(t3);
#endif

SamplerState basicSampler : register(s0);

struct PSOut {
    float4 color   : SV_Target0;
    float4 light   : SV_Target1;
};

PSOut psIn(PSIn input) : SV_Target{
    PSOut pso = (PSOut)0;
    float2 texCoord = input.uv;

#ifdef COPY
    pso.color = diffuseColor.Sample(basicSampler, texCoord);
    pso.light = lightColor.Sample(basicSampler, texCoord);
    return pso;
#endif

#ifndef DYNAMIC
    float zOverW = depthTexture.Sample(basicSampler, texCoord);
    // H is the viewport position at this pixel in the range -1 to 1.
    float4 H = float4(texCoord.x * 2 - 1, (1 - texCoord.y) * 2 - 1, zOverW, 1);
    // Transform by the view-projection inverse.
    float4 D = mul(H, mainConstants.viewProjectionInverse);
    // Divide by w to get the world position.
    float4 worldPos = D / D.w;


    
    // Current viewport position
    float4 currentPos = H;
    // Use the world position, and transform by the previous view-
    // projection matrix.
    float4 previousPos = mul(worldPos, mainConstants.past_viewProjection);
    // Convert to nonhomogeneous points [-1,1] by dividing by w.
    
    previousPos /= previousPos.w;
    // Use this frame's position and last frame's to compute the pixel
    // velocity.
    float2 velocity = (currentPos - previousPos)/2.f * motionBlurCosntBuffer.strength
    *mainConstants.dt;
    // Get the initial color at this pixel.
    float4 color = diffuseColor.Sample(basicSampler, texCoord);
    float4 bloomColor = bloomMask.Sample(basicSampler, texCoord);
    float hasBloom = saturate(dot(bloomColor.rgb,float3(1,1,1))*10000);
    color += motionBlurCosntBuffer.bloomStrength * color * hasBloom;
    float4 light = lightColor.Sample(basicSampler, texCoord);
    texCoord += velocity;
    
    
    float totalStrength = 1 + (motionBlurCosntBuffer.bloomStrength) * hasBloom;
    
    for(int i = 1; i < motionBlurCosntBuffer.numSampes; ++i, texCoord += velocity)
    {   // Sample the color buffer along the velocity vector.
        float4 currentColor = diffuseColor.Sample(basicSampler, texCoord);
        float4 currentBloomColor = bloomMask.Sample(basicSampler, texCoord);
        float4 currentLight = lightColor.Sample(basicSampler, texCoord);
        // Add the current color to our color sum.
        hasBloom = saturate(dot(currentBloomColor.rgb,float3(1,1,1))*10000);
        color += currentColor + motionBlurCosntBuffer.bloomStrength * currentColor * hasBloom;
        light += currentLight;
        totalStrength += 1 + (motionBlurCosntBuffer.bloomStrength) * hasBloom;
    }
    
    // Average all of the samples to get the final blur color.
    pso.color = color / totalStrength;
    pso.light = light / motionBlurCosntBuffer.numSampes;\
    pso.color.w = 1;
    pso.light.w = 1;

#endif
    
#ifdef DYNAMIC
    float blurSwitch = blurStrength.Sample(basicSampler, texCoord);
    float hasBlur = 0.0;
    if (blurSwitch > 1.5)
        hasBlur = 1.0;
    float2 velocity = velocityTexture.Sample(basicSampler, texCoord)
        * hasBlur
        * motionBlurCosntBuffer.strength
        * mainConstants.dt;
    // Get the initial color at this pixel.
    float4 color = diffuseColor.Sample(basicSampler, texCoord);
    float4 light = lightColor.Sample(basicSampler, texCoord);
    texCoord += velocity;
    
    for(int i = 1; i < motionBlurCosntBuffer.numSampes; ++i, texCoord += velocity)
    {   // Sample the color buffer along the velocity vector.
        float4 currentColor = diffuseColor.Sample(basicSampler, texCoord);
        float4 currentLight = lightColor.Sample(basicSampler, texCoord);
        // Add the current color to our color sum.
        color += currentColor;
        light += currentLight;
    }
    
    // Average all of the samples to get the final blur color.
    pso.color = color / motionBlurCosntBuffer.numSampes;
    pso.light = light / motionBlurCosntBuffer.numSampes;
    pso.color.w = saturate(saturate(blurSwitch - 0.5) + 0.5);
    pso.light.w = saturate(saturate(blurSwitch - 0.5) + 0.5);

#endif
    
    return pso;
}