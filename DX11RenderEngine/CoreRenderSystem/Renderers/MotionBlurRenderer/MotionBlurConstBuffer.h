#ifdef __cplusplus
#pragma once
#include "../../Utils/TransformUtils.h"
#include "../../CoreShaderInclude.h"
#endif

struct MotionBlurCosntBuffer {
	//matrix projection;
	int numSampes;
	float strength;
};

STRUCTURE(1, MotionBlurCosntBuffer, motionBlurCosntBuffer)