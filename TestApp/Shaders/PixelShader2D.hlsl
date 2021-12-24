struct PSIn {
	float4 position		: SV_POSITION;
	float2 uv			: TEXCOORD;
};




float4 main(PSIn input) : SV_Target
{
	//return tex.Sample(basicSampler, input.uv);
	return float4(input.uv.x, input.uv.y, 0.0f ,1.0f);
}