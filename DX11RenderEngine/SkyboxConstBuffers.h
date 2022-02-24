#ifdef __cplusplus
#pragma once
#include "TransformUtils.h"
#include "CoreShaderInclude.h"
#endif

struct SkyboxCosntBuffer {
	matrix projection;
	matrix view;
};

STRUCTURE(0, SkyboxCosntBuffer, SkyboxTransform)