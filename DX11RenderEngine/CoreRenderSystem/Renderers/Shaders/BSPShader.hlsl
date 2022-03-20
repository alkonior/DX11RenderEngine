#define HLSL
#include "P:/Quake-2/ref_dx11rg/DX11RenderEngine/DX11RenderEngine/CoreRenderSystem/CoreShaderInclude.h"
#include "P:/Quake-2/ref_dx11rg/DX11RenderEngine/DX11RenderEngine/CoreRenderSystem/Renderers/UPRenderer/UPConstBuffers.h"

struct VSIn {
	float3 pos     : Position;
	float2 uv      : TEXCOORD;
	float2 luv     : LIGHTTEXCOORD;
};


struct PSIn {
	float4 pos  : SV_Position;
	float2 uv   : TEXCOORD;
	float2 luv  : LIGHTTEXCOORD;
};


//cbuffer externalData {
//	matrix world;
//	matrix view;
//	matrix projection;
//	float4 color;
//	float2 texOffset;
//}



PSIn vsIn(VSIn input) {
	PSIn vso = (PSIn)0;

	float3 worldPos = input.pos;

	vso.pos = mul(mul(float4(worldPos, 1.0f), upCosntBuffer.world), mainConstants.viewProjection);
	vso.uv = input.uv - upCosntBuffer.texOffset;
	vso.luv = input.luv;// ;

	return vso;
}


Texture2D tex : register(t0); 
Texture2D lightMapText : register(t1);

SamplerState basicSampler : register(s0);
SamplerState lightSampler : register(s1);


struct PSOut {
	float4 color   : SV_Target0;

	float4 light   : SV_Target1;
	
	float4 alpha   : SV_Target2;
	
};


PSOut psIn(PSIn input) : SV_Target
{
	PSOut pso = (PSOut)0;
	
#ifdef RED
	pso.color = float4(1.0, 0, 0, 1.0f);
	pso.light = float4(1.0, 0, 0, 1.0f);
	pso.alpha = float4(1.0, 0, 0, 1.0f);
	pso.worldPos = input.worldPos;
	return pso;
#endif

#ifdef COLORED
	pso.color = color;
	pso.light = float4(1, 1, 1, 1);
	return pso;
#endif


#ifdef ALPHA
	pso.alpha = tex.Sample(basicSampler, input.uv);
#ifdef LIGHTMAPPED
	pso.alpha *= lightMapText.Sample(lightSampler, input.luv);
#endif
#else
	pso.color = tex.Sample(basicSampler, input.uv) ;
#ifdef LIGHTMAPPED
	pso.light = lightMapText.Sample(lightSampler, input.luv);
#endif 
#endif 

	return pso;
}




//VertexShaderOutput main(float2 pos : Position, float3 color : Color) {
//	VertexShaderOutput vso;
//	//matrix worldViewProj = mul(mul(world, view), projection);
//	//vso.pos = mul(float4(pos.x, pos.y, 0.0f, 1.0f), worldViewProj);
//	vso.pos = float4(pos.x, pos.y, 0.0f, 1.0f);
//	vso.color = float4(color, 1.0);
//	return vso;
//}