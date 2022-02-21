#ifdef __cplusplus
#pragma once
#include "TransformUtils.h"
#include "CoreShaderInclude.h"
#endif

struct Shader2DCosntBuffer {
	matrix transform;
	float2 uvShift;
	float2 uvScale;
	float4 color;
};

STRUCTURE(0, Shader2DCosntBuffer, UITransform)