#pragma once
#include "Utils.h"



namespace Renderer
{

enum StencilOperation {
	STENCILOPERATION_KEEP,
	STENCILOPERATION_ZERO,
	STENCILOPERATION_REPLACE,
	STENCILOPERATION_INCREMENT,
	STENCILOPERATION_DECREMENT,
	STENCILOPERATION_INCREMENTSATURATION,
	STENCILOPERATION_DECREMENTSATURATION,
	STENCILOPERATION_INVERT
};

enum CompareFunction {
	FNA3D_COMPAREFUNCTION_ALWAYS,
	FNA3D_COMPAREFUNCTION_NEVER,
	FNA3D_COMPAREFUNCTION_LESS,
	FNA3D_COMPAREFUNCTION_LESSEQUAL,
	FNA3D_COMPAREFUNCTION_EQUAL,
	FNA3D_COMPAREFUNCTION_GREATEREQUAL,
	FNA3D_COMPAREFUNCTION_GREATER,
	FNA3D_COMPAREFUNCTION_NOTEQUAL
};

struct DepthStencilState {
	uint8_t depthBufferEnable;
	uint8_t depthBufferWriteEnable;
	CompareFunction depthBufferFunction;
	uint8_t stencilEnable;
	int32_t stencilMask;
	int32_t stencilWriteMask;
	uint8_t twoSidedStencilMode;
	StencilOperation stencilFail;
	StencilOperation stencilDepthBufferFail;
	StencilOperation stencilPass;
	CompareFunction stencilFunction;
	StencilOperation ccwStencilFail;
	StencilOperation ccwStencilDepthBufferFail;
	StencilOperation ccwStencilPass;
	CompareFunction ccwStencilFunction;
	int32_t referenceStencil;
};

};