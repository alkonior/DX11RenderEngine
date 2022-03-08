struct VertexShaderInput {
	float2 pos : Position;
	float4 color : Color;
};

struct VertexShaderOutput {
	float4 pos : SV_Position;
	float4 color : Color;
};

cbuffer externalData {
	matrix world;
	matrix view;
	matrix projection;
};

struct PSIn {
	float3 color : Color;
	float4 pos : SV_Position;
};

PSIn main(float2 pos : Position, float3 color : Color) {
	PSIn vso;
	vso.pos = float4(pos.x, pos.y, 0.0f, 1.0f);
	vso.color = color;
	return vso;
}

//VertexShaderOutput main(float2 pos : Position, float3 color : Color) {
//	VertexShaderOutput vso;
//	//matrix worldViewProj = mul(mul(world, view), projection);
//	//vso.pos = mul(float4(pos.x, pos.y, 0.0f, 1.0f), worldViewProj);
//	vso.pos = float4(pos.x, pos.y, 0.0f, 1.0f);
//	vso.color = float4(color, 1.0);
//	return vso;
//}