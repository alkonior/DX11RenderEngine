#ifdef __cplusplus
#pragma once
#include "TransformUtils.h"
#include "CoreRenderSystem/CoreShaderInclude.h"
#endif

struct BloomCosntBuffer {
	int radius;
	float sigma;
	float threshold;
	float intensity;
};

STRUCTURE(1, BloomCosntBuffer, BloomCosntants)