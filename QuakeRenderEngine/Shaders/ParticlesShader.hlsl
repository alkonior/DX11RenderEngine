#define HLSL

#include "ref_dx11rg\DX11RenderEngine\DX11RenderEngine\include/CoreRenderSystem/CoreShaderInclude.h"
#include "ref_dx11rg\DX11RenderEngine\QuakeRenderEngine/source/RendererPasses/TAARenderPass\TAARenderPassConstBuffer.h"

struct VSIn {
	float3 pos     : Position;
	float4 color   : Color;
};

struct GSIn {
	float4 pos     : Position;
	float4 color   : Color;
};

struct PSIn {
	float4 pos     : SV_Position;
	float4 color   : Color;
};


GSIn vsIn(VSIn input) {
	GSIn vso;

	float4 worldPosition = float4(input.pos, 1);

	vso.pos = mul(worldPosition, coreConstants.currentMatrices.view);

	vso.color = input.color;
	return vso;
}

PSIn CreateQuadVertex(GSIn input, float2 offset) {
	PSIn gso = (PSIn)0;
	input.pos.xy += offset;
	gso.pos = mul(input.pos, coreConstants.currentMatrices.projection);

	gso.pos.xy += coreConstants.taaBuffer.taaStrength*
	coreConstants.taaBuffer.taaPixelShift*
		gso.pos.w;
	
	gso.color = input.color;
	return gso;
}


[maxvertexcount(4)]
void gsIn(point GSIn input[1], inout TriangleStream<PSIn> tristream) {
	tristream.Append(CreateQuadVertex(input[0], float2(1, -1)));
	tristream.Append(CreateQuadVertex(input[0], float2(1, 1)));
	tristream.Append(CreateQuadVertex(input[0], float2(-1, -1)));
	tristream.Append(CreateQuadVertex(input[0], float2(-1, 1)));
	tristream.RestartStrip();

}



float4 psIn(PSIn input) : SV_Target
{
#ifdef RED
	return float4(1.0, 0,0, 1.0f);
#endif

	return float4(input.color.rgb, 0.3);
}

