#pragma once
#include "IRenderer/Utils.h"
namespace Renderer
{
enum Blend {
	BLEND_ONE,
	BLEND_ZERO,
	BLEND_SOURCECOLOR,
	BLEND_INVERSESOURCECOLOR,
	BLEND_SOURCEALPHA,
	BLEND_INVERSESOURCEALPHA,
	BLEND_DESTINATIONCOLOR,
	BLEND_INVERSEDESTINATIONCOLOR,
	BLEND_DESTINATIONALPHA,
	BLEND_INVERSEDESTINATIONALPHA,
	BLEND_BLENDFACTOR,
	BLEND_INVERSEBLENDFACTOR,
	BLEND_SOURCEALPHASATURATION
};

enum BlendFunction {
	BLENDFUNCTION_ADD,
	BLENDFUNCTION_SUBTRACT,
	BLENDFUNCTION_REVERSESUBTRACT,
	BLENDFUNCTION_MAX,
	BLENDFUNCTION_MIN
};

enum ColorWriteChannels : std::uint32_t {
	COLORWRITECHANNELS_NONE = 0,
	COLORWRITECHANNELS_RED = 1,
	COLORWRITECHANNELS_GREEN = 2,
	COLORWRITECHANNELS_BLUE = 4,
	COLORWRITECHANNELS_ALPHA = 8,
	COLORWRITECHANNELS_ALL = 15
};

struct BlendState {

	bool enabled = false;
	Blend colorSourceBlend;
	Blend colorDestinationBlend;
	BlendFunction colorBlendFunction;
	Blend alphaSourceBlend;
	Blend alphaDestinationBlend;
	BlendFunction alphaBlendFunction;
	std::uint32_t colorWriteEnable;
	std::uint32_t colorWriteEnable1;
	std::uint32_t colorWriteEnable2;
	std::uint32_t colorWriteEnable3;
	Color blendFactor;
	int32_t multiSampleMask;
};

};