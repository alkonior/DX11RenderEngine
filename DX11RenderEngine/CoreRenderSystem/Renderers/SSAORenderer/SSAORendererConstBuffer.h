#ifdef __cplusplus
#pragma once
#include "../../Utils/TransformUtils.h"
#include "../../CoreShaderInclude.h"

//enum TAADebugFlags {
//	MarkNoHistoryPixels = 1,
//	AllowDepthThreshold = 2,
//	AllowBicubicFilter = 4,
//	AllowVarianceClipping = 8,
//	AllowYCoCg = 16,
//	AllowNeighbourhoodSampling = 32,
//	AllowLongestVelocityVector = 64,
//};

#endif

#define SSAO_NUM_THREADS_X 8
#define SSAO_NUM_THREADS_Y 8



struct SSAOCosntBuffer
{
    float4x4 gProjTex;
    float4   gOffsetVectors[14];

    // For SsaoBlur.hlsl
    float4 gBlurWeights[8];

    float2 gInvRenderTargetSize; // 1/width, 1/ height

    // Coordinates given in view space.
    float    gOcclusionRadius   = 0.5;
    float    gOcclusionFadeStart = 0.2;
    float    gOcclusionFadeEnd   = 1.0;
    float    gSurfaceEpsilon   = 0.05;
};

STRUCTURE(1, SSAOCosntBuffer, SSAOData)