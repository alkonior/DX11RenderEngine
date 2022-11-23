#pragma once
#include <cstdint>



enum OpaqueDefines {
	OZERO = 0,
	ORED = 1,
	ODIFFUSE = 2,
	OMETALIC = 4,
	OROUGHNESS = 8,
	ONORMAL = 16,
};

enum class PixelFlagsOpaque
{
	NONE = 1 << 0,
	INVERT_NORMAL = 1 << 1,
};


enum class VertexFlagsOpaque {
	NONE = 1 << 0,
	SKINNED_MESH = 1 << 1
};


enum class VertexFlagsLighting
{
	NONE = 1 << 0,
	SCREEN_QUAD = 1 << 1,
};


enum class PixelFlagsLighting : uint
{
	AmbientLight = 1 << 0,
	DirectionalLight = 1 << 1,
	PointLight = 1 << 2,
	SpotLight = 1 << 3,
};


enum class PSToneMapFlags
{
	NONE = 1 << 0,
	CopyColor = 1 << 1,
	Simple = 1 << 2,
	WithLum = 1 << 3,
	WithLumEx = 1 << 4,
};


enum class VSToneMapFlags
{
	NONE = 1 << 0,
};



constexpr enum class PixelFlagsOpaque operator|(const enum class PixelFlagsOpaque& selfValue, const enum class PixelFlagsOpaque& inValue)
{
	return static_cast<enum class PixelFlagsOpaque>(uint32_t(selfValue) | uint32_t(inValue));
}

constexpr enum class VertexFlagsOpaque operator|(const enum class VertexFlagsOpaque& selfValue, const enum class VertexFlagsOpaque& inValue)
{
	return static_cast<enum class VertexFlagsOpaque>(uint32_t(selfValue) | uint32_t(inValue));
}

constexpr enum class VertexFlagsLighting operator|(const enum class VertexFlagsLighting& selfValue, const enum class VertexFlagsLighting& inValue)
{
	return static_cast<enum class VertexFlagsLighting>(uint32_t(selfValue) | uint32_t(inValue));
}

constexpr enum class PixelFlagsLighting operator|(const enum class PixelFlagsLighting& selfValue, const enum class PixelFlagsLighting& inValue)
{
	return static_cast<enum class PixelFlagsLighting>(uint32_t(selfValue) | uint32_t(inValue));
}

constexpr enum class PSToneMapFlags operator|(const enum class PSToneMapFlags& selfValue, const enum class PSToneMapFlags& inValue)
{
	return static_cast<enum class PSToneMapFlags>(uint32_t(selfValue) | uint32_t(inValue));
}

constexpr enum class VSToneMapFlags operator|(const enum class VSToneMapFlags& selfValue, const enum class VSToneMapFlags& inValue)
{
	return static_cast<enum class VSToneMapFlags>(uint32_t(selfValue) | uint32_t(inValue));
}

