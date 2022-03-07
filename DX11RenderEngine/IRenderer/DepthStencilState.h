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
	COMPAREFUNCTION_ALWAYS,
	COMPAREFUNCTION_NEVER,
	COMPAREFUNCTION_LESS,
	COMPAREFUNCTION_LESSEQUAL,
	COMPAREFUNCTION_EQUAL,
	COMPAREFUNCTION_GREATEREQUAL,
	COMPAREFUNCTION_GREATER,
	COMPAREFUNCTION_NOTEQUAL
};

struct DepthStencilState {
	uint8_t depthBufferEnable = false;
	uint8_t depthBufferWriteEnable = true;
	CompareFunction depthBufferFunction = CompareFunction::COMPAREFUNCTION_GREATER;
	uint8_t stencilEnable = false;
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