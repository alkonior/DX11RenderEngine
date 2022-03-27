#ifdef __cplusplus
#pragma once
#include "../../Utils/TransformUtils.h"
#include "../../CoreShaderInclude.h"
#endif

struct TAACosntBuffer {
	int numSamples;
	float depthThreshold;
	int DilationMode;
	int ReprojectionMode;
	int NeighborhoodClampMode;
};

STRUCTURE(1, TAACosntBuffer, TAABuffer)