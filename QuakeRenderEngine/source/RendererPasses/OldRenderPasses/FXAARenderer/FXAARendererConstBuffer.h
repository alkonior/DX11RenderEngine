#ifdef __cplusplus
#pragma once
#include "TransformUtils.h"
#include "CoreRenderSystem/CoreShaderInclude.h"
#endif

struct FXAACosntBuffer {
	float fxaaQualitySubpix				;
	float fxaaQualityEdgeThreshold		;
	float fxaaQualityEdgeThresholdMin	;
	float fxaaConsoleEdgeSharpness		;
	float fxaaConsoleEdgeThreshold		;
	float fxaaConsoleEdgeThresholdMin	;
};

STRUCTURE(1, FXAACosntBuffer, FXAABuffer)