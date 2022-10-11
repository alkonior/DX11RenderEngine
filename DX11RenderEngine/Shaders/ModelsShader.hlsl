#define HLSL
#include "..\..\DX11RenderEngine\DX11RenderEngine\source\CoreRenderSystem\CoreShaderInclude.h"
#include "..\..\DX11RenderEngine\DX11RenderEngine\source\CoreRenderSystem\Renderers\ModelRenderer\ModelConstBuffers.h"


struct VSIn {
	float3 pos : Position;
	float3 normal : NORMAL;
	float2 uv  : TEXCOORD;
};


struct PSIn {
	float4 pos			: SV_Position;
	float2 uv			: TEXCOORD;
	float3 normal		: NORMAL;
	float4 oldWorldPos	: OLDWPOS;
	float4 worldPos	    : NEWWPOS;
};



PSIn vsIn(VSIn input) {
	PSIn vso = (PSIn)0;
	float3 worlPos;
	matrix worldMat; 
	float3 oldWorlPos;
	matrix oldWorldMat; 

	worlPos = input.pos;
	worldMat = modelsCosntBuffer.world;
	oldWorlPos = input.pos;
	oldWorldMat = modelsCosntBuffer.world;

	//vso.oldPos = oldWorlPos;
	//vso.newPos = worlPos;
	vso.pos = mul(mul(float4(worlPos, 1.0f), worldMat), mainConstants.viewProjection);
	vso.oldWorldPos = mul(float4(oldWorlPos, 1.0f), oldWorldMat);
	vso.worldPos = mul(float4(worlPos, 1.0f), worldMat);
	vso.pos.z = 0; 
	vso.pos.xy += taaShiftBuffer.taaStrength*
	taaShiftBuffer.taaPixelShift*
		vso.pos.w;
	
	//vso.oldPixelPos = mul(float4(worlPos, 1.0f), worldMat);
	//vso.newPixelPos = mul(float4(oldWorlPos, 1.0f), oldWorldMat);
	

	vso.normal = input.normal;

	vso.normal = mul(float4(vso.normal, 0.0f), worldMat);
	
	vso.uv = input.uv;


	return vso;
}


Texture2D tex : register(t0);

SamplerState basicSampler : register(s0);


struct PSOut {
	float4 color      : SV_Target0;
	//float4 light      : SV_Target1;
	//packed_velocity_t velocity   : SV_Target2;
	//float  blurMask   : SV_Target3;
	//float4  normal   : SV_Target4;
};

PSOut psIn(PSIn input) : SV_Target
{
	PSOut pso = (PSOut)0;
	
	float4 curPixelPos = mul(input.worldPos, mainConstants.viewProjection)*0.5+0.5;
	float4 oldPixelPos = mul(input.oldWorldPos, mainConstants.past_viewProjection)*0.5+0.5;
	//pso.velocity = PackVelocity((curPixelPos/curPixelPos.w - oldPixelPos/oldPixelPos.w));
	
	//if (dot(input.normal, input.normal) > 0.00001)
	//	pso.normal.xyz = input.normal;
	
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