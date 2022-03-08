#ifdef __cplusplus
#pragma once
#include "../../Utils/TransformUtils.h"
#include "../../CoreShaderInclude.h"
#endif

struct FXAACosntBuffer {
	float fxaaQualitySubpix				;
	float fxaaQualityEdgeThreshold		;
	float fxaaQualityEdgeThresholdMin	;
	float fxaaConsoleEdgeSharpness		;
	float fxaaConsoleEdgeThreshold		;
	float fxaaConsoleEdgeThresholdMin	;
};

STRUCTURE(0, FXAACosntBuffer, FXAABuffer)