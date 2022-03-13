#ifdef __cplusplus
#pragma once
#include "Utils/TransformUtils.h"
#include "CoreShaderInclude.h"
#endif

struct BloomCosntBuffer {
	int radius;
	float sigma;
	float threshold;
	float intensity;
};

STRUCTURE(1, BloomCosntBuffer, BloomCosntants)