#define HLSL
#include "P:\Quake-2\ref_dx11rg\DX11RenderEngine\DX11RenderEngine\CoreRenderSystem/CoreShaderInclude.h"
#include "P:\Quake-2\ref_dx11rg\DX11RenderEngine\DX11RenderEngine\CoreRenderSystem/Renderers/TAARenderer\TAARendererConstBuffer.h"

struct VSIn
{
    float2 pos : Position;
    float2 uv : TEXCOORD;
};

struct PSIn
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
};


PSIn vsIn(VSIn input)
{
    PSIn output = (PSIn)0;

    output.pos = float4(input.pos, 0.0f, 1.0f);
    output.uv = input.uv;
    return output;
}


#ifndef COPY

Texture2D curColor  : register(t0);

Texture2D curDepth  : register(t1);
Texture2D pastColor : register(t2);
Texture2D pastDepth : register(t3);
Texture2D velocityField  : register(t4);

float3 rgb_to_ycbcr(float3 rgb) {
    float y = 0.299 * rgb.x + 0.587 * rgb.y + 0.114 * rgb.z;
    float cb = (rgb.z - y) * 0.565;
    float cr = (rgb.x - y) * 0.713;

    return float3(y, cb, cr);
}

float3 ycbcr_to_rgb(float3 yuv) {
    return float3(
        yuv.x + 1.403 * yuv.z,
        yuv.x - 0.344 * yuv.y - 0.714 * yuv.z,
        yuv.x + 1.770 * yuv.y
    );
}

SamplerState basicSampler : register(s0);
SamplerState linearSampler : register(s1);


float3 getColor(float2 texCord, SamplerState smpl, Texture2D tex)
{
#ifdef  USEYUV
	return rgb_to_ycbcr(tex.Sample(smpl, texCord));
#else
	return tex.Sample(smpl, texCord);
#endif
}

float3 lerp(float alpha, float3 left, float3 right)
{
    return left*(1-alpha)+alpha*right;
}


float3 ClipAABB(float3 aabbMin, float3 aabbMax, float3 prevSample, float3 avg)
{
#if 1
	// note: only clips towards aabb center (but fast!)
	float3 p_clip = 0.5 * (aabbMax + aabbMin);
	float3 e_clip = 0.5 * (aabbMax - aabbMin);

	float3 v_clip = prevSample - p_clip;
	float3 v_unit = v_clip.xyz / e_clip;
	float3 a_unit = abs(v_unit);
	float ma_unit = max(a_unit.x, max(a_unit.y, a_unit.z));

	if (ma_unit > 1.0)
		return p_clip + v_clip / ma_unit;
	else
		return prevSample;// point inside aabb
#else
	float3 r = prevSample - avg;
	float3 rmax = aabbMax - avg.xyz;
	float3 rmin = aabbMin - avg.xyz;

	const float eps = 0.000001f;

	if (r.x > rmax.x + eps)
		r *= (rmax.x / r.x);
	if (r.y > rmax.y + eps)
		r *= (rmax.y / r.y);
	if (r.z > rmax.z + eps)
		r *= (rmax.z / r.z);

	if (r.x < rmin.x - eps)
		r *= (rmin.x / r.x);
	if (r.y < rmin.y - eps)
		r *= (rmin.y / r.y);
	if (r.z < rmin.z - eps)
		r *= (rmin.z / r.z);

	return avg + r;
#endif
}


static float FilterTriangle(in float x)
{
	return saturate(1.0f - x);
}

float FilterBlackmanHarris(in float x)
{
	x = 1.0f - x;

	const float a0 = 0.35875f;
	const float a1 = 0.48829f;
	const float a2 = 0.14128f;
	const float a3 = 0.01168f;
	const float PI = 3.14159265359f;
	return saturate(a0 - a1 * cos(PI * x) + a2 * cos(2 * PI * x) - a3 * cos(3 * PI * x));
}

float Filter(in float x, in int filterMode)
{
	if (filterMode == 1)
	{
		return FilterTriangle(x);
	}
	else if (filterMode == 2)
	{
		return FilterBlackmanHarris(x);
	}

	return x;
}

float2 LoadVelocity(float2 screenPos)
{
	float2 velocity = 0;

	int DilationMode = TAABuffer.DilationMode;
	
	float w,h;
	velocityField.GetDimensions(w,h);
	float2 shift = float2(1.0,1.0) / float2(w,h);

	if (DilationMode == 0)
	{
		velocity = velocityField.Sample(basicSampler, screenPos) + taaShiftBuffer.taaPixelShift;
	}
	else if (DilationMode == 1) // nearest depth
	{
		float closestDepth = 10.0f;
		for (int y = -1; y <= 1; y++)
		{
			for (int x = -1; x <= 1; x++)
			{
				float2 neighborVelocity = velocityField.Sample(basicSampler, screenPos + shift*int2(x, y)) + taaShiftBuffer.taaPixelShift;
				float neighborDepth = curDepth.Sample(basicSampler, screenPos + shift*int2(x, y));

				if (neighborDepth < closestDepth)
				{
					velocity = neighborVelocity;
					closestDepth = neighborDepth;
				}
			}
		}
	}
	else if (DilationMode == 2) // greatest velocity
	{
		float greatestVelocity = -1.0f;
		for (int y = -1; y <= 1; y++)
		{
			for (int x = -1; x <= 1; x++)
			{
				float2 neighborVelocity = velocityField.Sample(basicSampler, screenPos + shift*int2(x, y));
				float neighborVelocityMag = dot(neighborVelocity, neighborVelocity);
				if (dot(neighborVelocity, neighborVelocity) > greatestVelocity)
				{
					velocity = neighborVelocity;
					greatestVelocity = neighborVelocityMag;
				}
			}
		}
	}

	return velocity;
}

float4 psIn(PSIn input) : SV_Target
{
	/*
    float2 texCords = input.uv;
    float4 color = curColor.Sample(basicSampler, texCords);
    float2 velocity = velocityField.Sample(basicSampler, texCords);
    //velocity.y*=-1;
    float4 pColor = pastColor.Sample(basicSampler, texCords + velocity);

    float depth  = curDepth.Sample(basicSampler, texCords);
    float pDepth = pastDepth.Sample(basicSampler, texCords + velocity);
    
    float alpha = 1.0 -  1.0 / TAABuffer.numSamples;
    
    if (abs(depth-pDepth) > TAABuffer.depthThreshold)
        alpha = 0;

*/
    float2 texCords = input.uv;

	float3 clrMin = 99999999.0f;
	float3 clrMax = -99999999.0f;

	float3 m1 = 0.0f;
	float3 m2 = 0.0f;
	float mWeight = 0.0f;

	float depth = curDepth.Sample(basicSampler, texCords);

	float4 clipPos = float4(2 * texCords - 1, depth, 1);
	clipPos.y = -clipPos.y;

	float4 worldPos = mul(mainConstants.viewProjectionInverse, clipPos);
	worldPos.xyzw /= worldPos.w;

	float4 clipPosPrev = mul(mainConstants.past_viewProjection, worldPos);
	clipPosPrev.xy /= clipPosPrev.w;

	float2 uvPrev = clipPosPrev.xy * float2(0.5, -0.5) + float2(0.5, 0.5);

	//float2 historyUV1 = uvPrev + JitterOffset;

	//load velocity
	float2 historyUV = texCords - LoadVelocity(texCords);

	//outputRT[screenPos] = float4(10000*abs(historyUV- historyUV1), 0, 1);
	//return;

	//find area min max
	int SampleRadius = 1;
	float w,h;
	curColor.GetDimensions(w,h);
	float2 shift = float2(1.0,1.0) / float2(w,h);


	for (int y = -SampleRadius; y <= SampleRadius; ++y)
	{
		for (int x = -SampleRadius; x <= SampleRadius; ++x)
		{
			//float2 samplePos = screenPos + float2(x, y);

			float3 tap = getColor(texCords + shift*int2(x, y), basicSampler, curColor);

			clrMin = min(clrMin, tap);
			clrMax = max(clrMax, tap);

			m1 += tap;
			m2 += tap * tap;
			mWeight += 1.0f;
		}
	}

	float3 current = getColor(texCords, basicSampler, curColor);
	
	float3 history;
	
	int ReprojectionMode = TAABuffer.ReprojectionMode;

	if (ReprojectionMode == 0)
	{
		history = getColor(texCords, basicSampler, pastColor);
			//historyBuffer[screenPos].rgb;
	}
	else if (ReprojectionMode == 1)
	{
		history = getColor(texCords, linearSampler, pastColor);
	}
	else
	{
		float3 sum = 0.0f;
		float totalWeight = 0.0f;
		int ReprojectionFilter = 1;

		float2 reprojectedPos = historyUV * w * h;
		for (int y = -1; y <= 1; y++)
		{
			for (int x = -1; x <= 1; x++)
			{
				float2 samplePos = floor(reprojectedPos + float2(x, y)) + 0.5f;
				float3 reprojectedSample = getColor(texCords + shift*int2(x, y), linearSampler, pastColor);
				
				float2 sampleDist = abs(samplePos - reprojectedPos);
				float filterWeight = Filter(sampleDist.x, ReprojectionFilter) * Filter(sampleDist.y, ReprojectionFilter);

				//float sampleLum = Luminance(reprojectedSample);

				//if (UseExposureFiltering)
				//	sampleLum *= exp2(ManualExposure - ExposureScale + ExposureFilterOffset);

				//if (InverseLuminanceFiltering)
				//	filterWeight *= 1.0f / (1.0f + sampleLum);

				sum += reprojectedSample * filterWeight;
				totalWeight += filterWeight;
			}
		}

		history = max(sum / totalWeight, 0.0f);
	}

	int NeighborhoodClampMode = TAABuffer.NeighborhoodClampMode;

	if (NeighborhoodClampMode == 1) // RGB Clamp
	{
		history = clamp(history, clrMin, clrMax);
	}
	else if (NeighborhoodClampMode == 2 ) // RGB Clip
	{
		history = ClipAABB(clrMin, clrMax, history, m1 / mWeight);
	}
	else if (NeighborhoodClampMode == 3 )// Variance Clip
	{
		float VarianceClipGamma = 1;
		float3 mu = m1 / mWeight;
		float3 sigma = sqrt(abs(m2 / mWeight - mu * mu));
		float3 minc = mu - VarianceClipGamma * sigma;
		float3 maxc = mu + VarianceClipGamma * sigma;
		history = ClipAABB(minc, maxc, history, mu);
	}
	float3 result = lerp(1.0 -  1.0 / TAABuffer.numSamples, current, history);

#ifdef  USEYUV
	return float4(ycbcr_to_rgb(result), 1.0);
#else
	return float4(result, 1.0);
#endif
	
}

#else

Texture2D curColor : register(t0);
Texture2D curDepth : register(t1);

SamplerState basicSampler : register(s0);

struct PSO
{
    float4 newColor       : SV_Target0;
    float4 historyColor   : SV_Target1;
    float historyDepth   : SV_Target2;
};


PSO psIn(PSIn input) : SV_Target
{

    PSO pso = (PSO)0;

    float2 texCords = input.uv;
    pso.historyColor = curColor.Sample(basicSampler, texCords);
    pso.newColor = pso.historyColor;
    pso.historyDepth = curDepth.Sample(basicSampler, texCords);

    return  pso;
}

#endif
