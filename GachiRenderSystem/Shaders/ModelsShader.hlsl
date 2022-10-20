#define HLSL
#include "../DX11RenderEngine/DX11RenderEngine/include/CoreRenderSystem/CoreShaderInclude.h"
#include "../DX11RenderEngine/QuakeRenderEngine/source/RendererPasses/ModelsRenderPass/ModelsPassConstBuffer.h"

#ifdef LERP
struct VSIn {
	float2 uv  :     TEXCOORD;
	float3 pos1 :    Position1;
	float3 normal1 : NORMAL1;
	float3 pos2 :    Position2;
	float3 normal2 : NORMAL2;
};
#else
struct VSIn {
	float3 pos : Position;
	float3 normal : NORMAL;
	float2 uv  : TEXCOORD;
};
#endif


struct PSIn {
	float4 pos			: SV_Position;
	float2 uv			: TEXCOORD;
	float3 normal		: NORMAL;
	//float2 velocity		: VELOCITY;
	//float3 oldPos		: OLDPOS;
	//float3 newPos		: NEWPOS;
	float4 oldWorldPos	: OLDWPOS;
	float4 worldPos	    : NEWWPOS;
};

//matrix lerp(float alpha, matrix old, matrix next)
//{
//	return old * alpha + next * (1 - alpha) ;
//};
//
//float3 lerp(float alpha, float3 old,  float3 next)
//{
//	return old * alpha + next * (1 - alpha) ;
//}; 


PSIn vsIn(VSIn input) {
	PSIn vso = (PSIn)0;
	float3 worlPos;
	matrix worldMat; 
	float3 oldWorlPos;
	matrix oldWorldMat; 
#ifdef LERP
	worlPos = lerp( input.pos1, input.pos2, 1.0-modelsCosntBuffer.alpha);
	worldMat = lerp(modelsCosntBuffer.world, modelsCosntBuffer.oldWorld, modelsCosntBuffer.alpha);
	oldWorlPos = lerp(input.pos1, input.pos2, 1.0-modelsCosntBuffer.oldAlpha);
	oldWorldMat = lerp( modelsCosntBuffer.world, modelsCosntBuffer.oldWorld, modelsCosntBuffer.oldAlpha);
#else
	worlPos = input.pos;
	worldMat = modelsCosntBuffer.world;
	oldWorlPos = input.pos;
	oldWorldMat = modelsCosntBuffer.world;
#endif

	//vso.oldPos = oldWorlPos;
	//vso.newPos = worlPos;
	vso.pos = mul(mul(float4(worlPos, 1.0f), worldMat), coreConstants.currentMatrices.viewProjection);
	vso.oldWorldPos = mul(float4(oldWorlPos, 1.0f), oldWorldMat);
	vso.worldPos = mul(float4(worlPos, 1.0f), worldMat);

	vso.pos.xy += coreConstants.taaBuffer.taaStrength*
	coreConstants.taaBuffer.taaPixelShift*
		vso.pos.w;
	
	//vso.oldPixelPos = mul(float4(worlPos, 1.0f), worldMat);
	//vso.newPixelPos = mul(float4(oldWorlPos, 1.0f), oldWorldMat);
	

	
	

	

#ifdef LERP
	vso.normal = lerp( input.normal1 , input.normal2, 1.0-modelsCosntBuffer.alpha);
	//if (modelsCosntBuffer.alpha < 0.5) {
	//	vso.normal = input.normal1;
	//}
	//else {
	//	vso.normal = input.normal2;
	//}
#else
	vso.normal = input.normal;
#endif

	vso.normal = mul(float4(vso.normal, 0.0f), worldMat);

#ifdef BAD_UV
	vso.uv = input.uv / modelsCosntBuffer.wh;
	vso.uv.y = vso.uv.y;
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
	//packed_velocity_t velocity   : SV_Target2;
	float  blurMask   : SV_Target3;
	float4  normal   : SV_Target4;
};

PSOut psIn(PSIn input) : SV_Target
{
	PSOut pso = (PSOut)0;
	
	float4 curPixelPos = mul(input.worldPos, coreConstants.currentMatrices.viewProjection)*0.5+0.5;
	float4 oldPixelPos = mul(input.oldWorldPos, coreConstants.currentMatrices.viewProjection)*0.5+0.5;
	//pso.velocity = PackVelocity((curPixelPos/curPixelPos.w - oldPixelPos/oldPixelPos.w));
	
	if (dot(input.normal, input.normal) > 0.00001)
		pso.normal.xyz = input.normal;
	
	pso.blurMask = modelsCosntBuffer.blurSwitch;
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