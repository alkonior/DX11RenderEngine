#define HLSL
#include "../Quake-2/ref_dx11rg/DX11RenderEngine/DX11RenderEngine/source/CoreRenderSystem/CoreShaderInclude.h"
#include "../Quake-2/ref_dx11rg/DX11RenderEngine/DX11RenderEngine/source/CoreRenderSystem/Renderers/UPRenderer/UPConstBuffers.h"

struct VSIn {
	float3 pos     : Position;
	float3 normal  : NORMAL;
	float2 uv      : TEXCOORD;
	float2 luv     : LIGHTTEXCOORD;
};


struct PSIn {
	float4 pos          : SV_Position;
	float2 uv           : TEXCOORD;
	float2 luv          : LIGHTTEXCOORD;
	float4 worldPos     : WPOS;
	float3 normal       : NORMAL;
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
	
	vso.worldPos = mul(float4(input.pos, 1.0f), upCosntBuffer.world);
	vso.pos =    mul(vso.worldPos, mainConstants.viewProjection);
	
	vso.pos.xy += taaShiftBuffer.taaStrength*
		taaShiftBuffer.taaPixelShift*
		vso.pos.w;
	//float4 oldPos = mul(mul(float4(input.pos, 1.0f), upCosntBuffer.world), mainConstants.past_viewProjection);
	//vso.velocity = (vso.pos/vso.pos.w - oldPos/oldPos.w)/2.f;
	
	vso.uv = input.uv - upCosntBuffer.texOffset;
	vso.luv = input.luv;// ;
	vso.normal = mul(float4(input.normal, 0), upCosntBuffer.world);
	return vso;
}


Texture2D tex : register(t0); 
Texture2D lightMapText : register(t1);

SamplerState basicSampler : register(s0);
SamplerState lightSampler : register(s1);


#ifndef ALPHA
struct PSOut {
	float4 color   : SV_Target0;
	float4 light   : SV_Target1;
	float4 alpha   : SV_Target2;
	packed_velocity_t velocity: SV_Target3;
	float4 normal: SV_Target4;
};


PSOut psIn(PSIn input) 
{
	PSOut pso = (PSOut)0;
	
	float4 curPixelPos = mul(input.worldPos, mainConstants.viewProjection)*0.5+0.5;
	float4 oldPixelPos = mul(input.worldPos, mainConstants.past_viewProjection)*0.5+0.5;
	pso.velocity = PackVelocity((curPixelPos/curPixelPos.w - oldPixelPos/oldPixelPos.w));
	if (dot(input.normal, input.normal) > 0.00001)
		pso.normal.xyz = input.normal;

#ifdef RED
	pso.color = float4(1.0, 0, 0, 1.0f);
	pso.light = float4(1.0, 0, 0, 1.0f);
	pso.alpha = float4(1.0, 0, 0, 1.0f);
	pso.worldPos = input.worldPos;
	return pso;
#endif
	
	pso.light = float4(1, 1, 1, 1);

#ifdef COLORED
	pso.color = color;
	return pso;
#endif
	
	pso.color = tex.Sample(basicSampler, input.uv);

#ifdef LIGHTMAPPED
	pso.light = lightMapText.Sample(lightSampler, input.luv);
#endif 

	return pso;
}


#else

struct PSOut {
	float4 alpha   : SV_Target2;
};

PSOut psIn(PSIn input) {
	PSOut pso = (PSOut)0;
	
	float4 curPixelPos = mul(input.worldPos, mainConstants.viewProjection);
	pso.alpha = tex.Sample(basicSampler, input.uv);
#ifdef LIGHTMAPPED
	pso.alpha *= lightMapText.Sample(lightSampler, input.luv);
#endif
	pso.alpha.w = curPixelPos.z/curPixelPos.w;

	return pso;
}
#endif




//VertexShaderOutput main(float2 pos : Position, float3 color : Color) {
//	VertexShaderOutput vso;
//	//matrix worldViewProj = mul(mul(world, view), projection);
//	//vso.pos = mul(float4(pos.x, pos.y, 0.0f, 1.0f), worldViewProj);
//	vso.pos = float4(pos.x, pos.y, 0.0f, 1.0f);
//	vso.color = float4(color, 1.0);
//	return vso;
//}