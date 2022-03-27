#ifdef __cplusplus
#pragma once
#include "../../Utils/TransformUtils.h"
#include "../../CoreShaderInclude.h"
#endif

struct TAACosntBuffer {
	int numSamples;
	float depthThreshold;
};

STRUCTURE(1, TAACosntBuffer, TAABuffer)