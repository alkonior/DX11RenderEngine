#pragma once
#include "IRenderer/Utils.h"

namespace Renderer
{

enum TextureFilter {
	TEXTUREFILTER_LINEAR,
	TEXTUREFILTER_POINT,
	TEXTUREFILTER_ANISOTROPIC,
	TEXTUREFILTER_LINEAR_MIPPOINT,
	TEXTUREFILTER_POINT_MIPLINEAR,
	TEXTUREFILTER_MINLINEAR_MAGPOINT_MIPLINEAR,
	TEXTUREFILTER_MINLINEAR_MAGPOINT_MIPPOINT,
	TEXTUREFILTER_MINPOINT_MAGLINEAR_MIPLINEAR,
	TEXTUREFILTER_MINPOINT_MAGLINEAR_MIPPOINT
};

enum TextureAddressMode {
	TEXTUREADDRESSMODE_WRAP,
	TEXTUREADDRESSMODE_CLAMP,
	TEXTUREADDRESSMODE_MIRROR
};

struct SamplerState {
	TextureFilter filter;
	TextureAddressMode addressU;
	TextureAddressMode addressV;
	TextureAddressMode addressW;
	float mipMapLevelOfDetailBias;
	int32_t maxAnisotropy;
	int32_t minMipLevel = 0;
	int32_t maxMipLevel = 0;
};

};