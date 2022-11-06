#define HLSL
#define GACHI

#include "../DX11RenderEngine/DX11RenderEngine/include/CoreRenderSystem/CoreShaderInclude.h"
#include "../DX11RenderEngine/GachiRenderSystem/source/RendererPasses/DebugRenderPass/DebugPassConstBuffer.h"

#ifdef D2D
struct VSIn {
	float3 pos : Position;
};

struct PSIn {
	float4 pos				: SV_Position;
};

PSIn vsIn(VSIn input) {
	PSIn vso = (PSIn)0;
	
	float4 worldPos = mul(float4(input.pos, 1.0f), debugCosntBuffer.world);
	vso.pos = worldPos;
	
	return vso;
}

struct PSOut {
	float4 color      : SV_Target0;
};

Texture2D depthTex : register(t0);

PSOut psIn(PSIn input) : SV_Target
{
	PSOut pso = (PSOut)0;
	
	
	//pso.blurMask = modelsCosntBuffer.blurSwitch;

#ifdef RED
	pso.color = float4(1.0, 1.0, 1.0, 1.0f);
	return pso;
#endif


	pso.color = float4(debugCosntBuffer.color, 1.0f);
	//todo

	return pso;
}


#endif
#define D3D
#ifdef D3D

struct VSIn {
	float3 pos : Position;
};

struct PSIn {
	float4 pos				: SV_Position;
};

PSIn vsIn(VSIn input) {
	PSIn vso = (PSIn)0;
	
	float4 worldPos = mul(float4(input.pos, 1.0f), debugCosntBuffer.world);
	vso.pos = mul(worldPos, coreConstants.currentMatrices.viewProjection);
	
	return vso;
}

struct PSOut {
	float4 color      : SV_Target0;
};

Texture2D<float> depthTex : register(t0);

PSOut psIn(PSIn input) : SV_Target
{
	PSOut pso = (PSOut)0;
	
	
	//pso.blurMask = modelsCosntBuffer.blurSwitch;

#ifdef RED
	pso.color = float4(1.0, 1.0, 1.0, 1.0f);
	return pso;
#endif


	pso.color = float4(debugCosntBuffer.color, 1.0f);
	//todo
	float depth = depthTex.Load(int3(input.pos.x, input.pos.y, 0)); 
	if (input.pos.z > depth)
		pso.color.xyz = pso.color.yzx;

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