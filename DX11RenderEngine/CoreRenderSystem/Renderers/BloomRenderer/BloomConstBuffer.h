#pragma once
#ifdef __cplusplus
#pragma once
#include "Utils/TransformUtils.h"
#include "CoreShaderInclude.h"
#endif

struct BloomCosntBuffer {
	uint16_t radius;
	float sigma;
	float threshold;
	float intensity;
};

STRUCTURE(0, BloomCosntBuffer, BloomBuffer)