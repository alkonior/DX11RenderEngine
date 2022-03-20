#define HLSL
#include "P:\Quake-2\ref_dx11rg\DX11RenderEngine\DX11RenderEngine\CoreRenderSystem/CoreShaderInclude.h"
#include "P:\Quake-2\ref_dx11rg\DX11RenderEngine\DX11RenderEngine\CoreRenderSystem/Renderers/ModelRenderer/ModelConstBuffers.h"



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
	float4 pos : SV_Position;
	float2 uv  : TEXCOORD;
	float3 normal : NORMAL;
};


PSIn vsIn(VSIn input) {
	PSIn vso = (PSIn)0;
	float3 worlPos;
#ifdef LERP
	worlPos = input.pos1 * modelsCosntBuffer.alpha + (1 - modelsCosntBuffer.alpha) * input.pos2;
#else
	worlPos = input.pos;
#endif

	vso.pos = mul(mul(mul(float4(worlPos, 1.0f), modelsCosntBuffer.world), mainConstants.view), mainConstants.projection);


#ifdef LERP
	if (modelsCosntBuffer.alpha < 0.5) {
		vso.normal = input.normal1;
	}
	else {
		vso.normal = input.normal2;
	}
#else
	vso.normal = input.normal;
#endif

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
	float4 color   : SV_Target0;
	float4 light   : SV_Target1;
};

PSOut psIn(PSIn input) : SV_Target
{
	PSOut pso = (PSOut)0;
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