struct PixelShaderInput {
	float4 position		: SV_POSITION;
	float4 color		: Color;
	//float2 uv			: TEXCOORD;
};


//Texture2D Texture : register(t0);


//SamplerState basicSampler : register(s0);


float4 main( float3 color : Color ) : SV_Target
{
	return float4( color,1.0f );
}