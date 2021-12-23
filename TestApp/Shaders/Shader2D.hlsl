struct VSIn {
	float2 pos : Position;
	float2 uv  : TEXCOORD;
};

struct VSOut {
	float4 pos : SV_Position;
	float2 uv  : TEXCOORD;
};


cbuffer externalData {
	matrix transform;
	float2 uvShift;
	float2 uvScale;
}


VSOut vsIn(VSIn input) {
	VSOut vso;
	vso.pos = mul(float4(input.pos.x, input.pos.y, 0.0f, 1.0f), transform);
	vso.uv = input.uv*uvScale+uvShift;
	return vso;
}


float4 psIn(VSOut input) : SV_Target
{
	//return tex.Sample(basicSampler, input.uv);
	return float4(input.uv.x, input.uv.y, 0.0f ,1.0f);
}




//VertexShaderOutput main(float2 pos : Position, float3 color : Color) {
//	VertexShaderOutput vso;
//	//matrix worldViewProj = mul(mul(world, view), projection);
//	//vso.pos = mul(float4(pos.x, pos.y, 0.0f, 1.0f), worldViewProj);
//	vso.pos = float4(pos.x, pos.y, 0.0f, 1.0f);
//	vso.color = float4(color, 1.0);
//	return vso;
//}