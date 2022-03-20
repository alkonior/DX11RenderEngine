#define HLSL
#include "P:\Quake-2\ref_dx11rg\DX11RenderEngine\DX11RenderEngine\CoreRenderSystem/CoreShaderInclude.h"
#include "P:\Quake-2\ref_dx11rg\DX11RenderEngine\DX11RenderEngine\CoreRenderSystem/Renderers/PPRenderer\PPConstBuffer.h"

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
Texture2D lightmap : register(t2);
Texture2D bloomMask : register(t1);
Texture2D depthTex : register(t3);

#ifdef ALPHA
Texture2D alphaSurfaces : register(t3);
#endif

SamplerState blureSampler : register(s0);
SamplerState pointSampler : register(s1);

float4 psIn(PSIn input) : SV_Target
{
    float2 texCoord = input.uv;

#ifdef BLOOMONLY
	return bloomMask.Sample(blureSampler, texCoord);
#endif
#ifdef COLORONLY
	return diffuseColor.Sample(blureSampler, texCoord);
#endif
#ifdef LIGHTONLY
	return lightmap.Sample(blureSampler, texCoord);
#endif
    
#ifdef ALPHAONLY
#ifdef ALPHA
    float4 alphaColor2 = alphaSurfaces.Sample(blureSampler, texCoord);
    //alphaColor.w = 0.3;
    return alphaColor2;
#endif
	return 0;
#endif

#ifdef ALPHA
	float4 alphaColor = alphaSurfaces.Sample(blureSampler, texCoord);
	alphaColor.w = 0.3;
	return alphaColor;
#endif

    float4 color = diffuseColor.Sample(blureSampler, texCoord);
    float4 light = lightmap.Sample(blureSampler, texCoord);
    light.rgb += float3(ppCosntBuffer.lightAdd,ppCosntBuffer.lightAdd,ppCosntBuffer.lightAdd);
    float4 bloom = bloomMask.Sample(blureSampler, texCoord);
    
    
    return color*light + bloom;

}
