#ifdef __cplusplus
#pragma once
#include "../../Utils/TransformUtils.h"
#include "../../CoreShaderInclude.h"

enum TAADebugFlags {
	MarkNoHistoryPixels = 1,
	AllowDepthThreshold = 2,
	AllowBicubicFilter = 4,
	AllowVarianceClipping = 8,
	AllowYCoCg = 16,
	AllowNeighbourhoodSampling = 32,
	AllowLongestVelocityVector = 64,
};

#endif

#define TAA_NUM_THREADS_X 8
#define TAA_NUM_THREADS_Y 8



struct TAACosntBuffer
{
	float4      Resolution;         //width, height, 1/width, 1/height
	float2      Jitter;
	uint        FrameNumber;
	uint        DebugFlags;         // AllowLongestVelocityVector | AllowNeighbourhoodSampling | AllowYCoCg | AllowVarianceClipping | AllowBicubicFilter | AllowDepthThreshold | MarkNoHistoryPixels
	int numSamples;
};

STRUCTURE(1, TAACosntBuffer, CBData)