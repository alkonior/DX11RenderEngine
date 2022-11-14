#define HLSL
#define GACHI

#include "../DX11RenderEngine/DX11RenderEngine/include/CoreRenderSystem/CoreShaderInclude.h"
#include "../DX11RenderEngine/GachiRenderSystem/source/RendererPasses/ModelsRenderPass/ModelsPassConstBuffer.h"


struct VSIn {
	float3 pos : Position;
	float3 normal : NORMAL;
	float2 uv  : TEXCOORD;
};

struct PSIn {
	float4 pos				: SV_Position;
	float2 uv				: TEXCOORD;
	float3 normal			: NORMAL;
	float2 velocity			: VELOCITY;
	float3 worldPos	    	: WORLDPOS;
	
	float2 worldSP			: WORLDSV;
	float2 oldWorldSP	    : OLDWORLDSV;
};

PSIn vsIn(VSIn input) {
	PSIn vso = (PSIn)0;
	

	//vso.oldPos = oldWorlPos;
	//vso.newPos = worlPos;
	float4 worldPos = mul(float4(input.pos, 1.0f), modelsCosntBuffer.world);
	vso.worldPos.xyz = (worldPos.xyz/worldPos.w).xyz;
	vso.pos = mul(worldPos, coreConstants.currentMatrices.viewProjection);
	float4 oldWorldSV =  mul(mul(float4(input.pos, 1.0f), modelsCosntBuffer.oldWorld), coreConstants.pastMatrices.viewProjection);
	
	vso.worldSP = (vso.pos/vso.pos.w).xy;
	vso.oldWorldSP = (oldWorldSV/oldWorldSV.w).xy;
	
	//vso.velocity =  PackVelocity((vso.pos/vso.pos.w - oldWorldPos/oldWorldPos.w)*0.5+0.5);
	

	vso.pos.xy += coreConstants.taaBuffer.taaStrength*
				  coreConstants.taaBuffer.taaPixelShift*
				  vso.pos.w;
	
	//vso.oldPixelPos = mul(float4(worlPos, 1.0f), worldMat);
	//vso.newPixelPos = mul(float4(oldWorlPos, 1.0f), oldWorldMat);
	

	
	


	//vso.normal = input.normal;

	vso.normal = mul(float4(vso.normal, 0.0f), modelsCosntBuffer.world).xyz;

#ifdef BAD_UV
	vso.uv = input.uv / modelsCosntBuffer.wh;
#else
	vso.uv = input.uv;
#endif


	return vso;
}


Texture2D tex : register(t0);

SamplerState basicSampler : register(s0);


struct PSOut {
	float4 color      : SV_Target0;
	float4 light      : SV_Target1;
	packed_velocity_t velocity   : SV_Target2;
	//float  blurMask   : SV_Target3;
	float4  normal   : SV_Target4;
	float4  worlpos   : SV_Target5;
};

PSOut psIn(PSIn input)
{
	PSOut pso = (PSOut)0;
	pso.velocity = PackVelocity(float3(input.velocity, 0));
	
	if (dot(input.normal, input.normal) > 0.00001)
		pso.normal.xyz = input.normal;
	pso.worlpos.xyz = input.worldPos.xyz;
	pso.worlpos.w = 1;
	
	//pso.blurMask = modelsCosntBuffer.blurSwitch;

#ifdef RED
	pso.color = float4(1.0, input.uv.x, input.uv.y, 1.0f);
	return pso;
#endif

#ifdef COLORED
	return color;
#endif

	
	pso.color = tex.Sample(basicSampler, input.uv);
#ifdef LIGHTED
	pso.light = modelsCosntBuffer.color;
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