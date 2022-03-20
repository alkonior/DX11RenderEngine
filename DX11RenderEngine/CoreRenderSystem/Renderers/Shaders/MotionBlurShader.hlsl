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



Texture2D diffuseColor : register(t0);
Texture2D lightColor : register(t1);
Texture2D<float> depthTexture : register(t2);

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
    float2 velocity = (currentPos - previousPos)/2.f * motionBlurCosntBuffer.strength;
    // Get the initial color at this pixel.
    float4 color = diffuseColor.Sample(basicSampler, texCoord);
    float4 light = lightColor.Sample(basicSampler, texCoord);
    texCoord += velocity;
    
    for(int i = 1; i < motionBlurCosntBuffer.numSampes; ++i, texCoord += velocity)
    {   // Sample the color buffer along the velocity vector.
        float4 currentColor = diffuseColor.Sample(basicSampler, texCoord);;
        float4 currentLight = lightColor.Sample(basicSampler, texCoord);;
        // Add the current color to our color sum.
        color += currentColor;
        light += currentLight;
    }
    
    // Average all of the samples to get the final blur color.
    pso.color = color / motionBlurCosntBuffer.numSampes;
    pso.light = light / motionBlurCosntBuffer.numSampes;
    
    return pso;
}