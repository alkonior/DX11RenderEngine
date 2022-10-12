#ifdef __cplusplus
#pragma once
#include "TransformUtils.h"
#include "CoreRenderSystem/CoreShaderInclude.h"
#endif

struct MotionBlurCosntBuffer {
	//matrix projection;
	int numSampes;
	float strength;
	float bloomStrength;
};

STRUCTURE(1, MotionBlurCosntBuffer, motionBlurCosntBuffer)