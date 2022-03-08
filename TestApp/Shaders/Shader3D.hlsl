#ifdef LERP
struct VSIn {
	float3 normal : NORMAL;
	float2 uv  : TEXCOORD;
	float3 pos1 : CurrentPosition;
	float3 pos2 : NextPosition;
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
};


cbuffer externalData {
	matrix world;
	matrix view;
	matrix projection;
}

#ifdef LERP
cbuffer LerpExternalData {
	float alpha;
}
#endif


PSIn vsIn(VSIn input) {
	PSIn vso;

#ifdef LERP
	float3 worldPosition = input.pos1 * alpha + (1 - alpha) * input.pos2;
#else 
	float3 worldPosition = input.pos;
#endif

	vso.pos = mul(mul(mul(float4(worldPosition, 1.0f), world), view), projection);
	vso.uv = input.uv;
	return vso;
}


Texture2D tex : register(t0);

SamplerState basicSampler : register(s0);


float4 psIn(PSIn input) : SV_Target
{
	return float4(1.0, 0,0, 1.0f);
#ifdef RED
	return float4(1.0, 0,0, 1.0f);
#endif
	//return float4(input.uv.x, input.uv.y, 0.0f ,1.0f);
	float4 color = tex.Sample(basicSampler, input.uv);
	float r = color.x;
	color.x = color.y;
	color.y = r;
	return color;
	//return float4(input.uv.x, input.uv.y, 0.0f ,1.0f);
}




//VertexShaderOutput main(float2 pos : Position, float3 color : Color) {
//	VertexShaderOutput vso;
//	//matrix worldViewProj = mul(mul(world, view), projection);
//	//vso.pos = mul(float4(pos.x, pos.y, 0.0f, 1.0f), worldViewProj);
//	vso.pos = float4(pos.x, pos.y, 0.0f, 1.0f);
//	vso.color = float4(color, 1.0);
//	return vso;
//}