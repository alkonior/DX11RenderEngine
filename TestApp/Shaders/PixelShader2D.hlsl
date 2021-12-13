struct PSIn {
	float4 position		: SV_POSITION;
	float2 uv			: TEXCOORD;
};


//Texture2D Texture : register(t0);
//SamplerState basicSampler : register(s0);


float4 main(PSIn input) : SV_Target
{
	return float4(input.uv.x, input.uv.y, 0.0f ,1.0f);
}