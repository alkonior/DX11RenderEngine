#pragma once
#include <cstdint>



namespace Renderer {

struct Texture {};
struct Buffer {};
struct ConstBuffer {};
struct Renderbuffer
{
	Texture* texture = nullptr;
};

struct VertexBufferBinding {
	size_t buffersCount = 0;
	Buffer** vertexBuffers = nullptr;
	unsigned int* vertexStride = nullptr;
	unsigned int* vertexOffset = nullptr;
};

struct MeshBindings {
	VertexBufferBinding vertexBuffer;
	Buffer* indexBuffer;
	uint8_t indexSize;
};

struct ShaderDefines {
	ShaderDefines(const char* name, const char* defenition) : name(name), defenition(defenition) {};
	ShaderDefines(const char* name) :ShaderDefines(name, "1") {};
	const char* name;
	const char* defenition;

};
struct PixelShader {};
struct VertexShader {};
struct ComputeShader {};
struct GeometryShader {};


struct Color {
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
	inline bool operator!= (const Color& rhs) { return ((r != rhs.r) || (g != rhs.g) || (b != rhs.b) || (a != rhs.a)); }
};

struct FColor {
	float x;
	float y;
	float z;
	float w;
};

struct Viewport {
	int32_t x;
	int32_t y;
	int32_t w;
	int32_t h;
	float minDepth;
	float maxDepth;
	bool operator == (const Viewport& vp) const
	{
		return
		(x == vp.x) &&
		(y == vp.y) &&
		(w == vp.w) &&
		(h == vp.h) &&
		(minDepth == vp.minDepth) &&
		(maxDepth == vp.maxDepth) ;
	}
	bool operator != (const Viewport& vp) const
	{
		return !(vp == *this);
	}
};

struct Rect {
	int32_t x;
	int32_t y;
	int32_t w;
	int32_t h;
};

enum PrimitiveType {
	PRIMITIVETYPE_TRIANGLELIST,
	PRIMITIVETYPE_TRIANGLESTRIP,
	PRIMITIVETYPE_LINELIST,
	PRIMITIVETYPE_LINESTRIP,
	PRIMITIVETYPE_POINTLIST_EXT,
	PRIMITIVETYPE_UNKNOWN
};

enum DepthFormat {
	DEPTHFORMAT_NONE,
	DEPTHFORMAT_D16,
	DEPTHFORMAT_D24,
	DEPTHFORMAT_D24S8,
	DEPTHFORMAT_D32
};

enum BufferUsage {
	BUFFERUSAGE_NONE,
	BUFFERUSAGE_WRITEONLY
};

enum SetDataOptions {
	SETDATAOPTIONS_NONE,
	SETDATAOPTIONS_DISCARD,
	SETDATAOPTIONS_NOOVERWRITE
};

enum DisplayOrientation {
	DISPLAYORIENTATION_DEFAULT,
	DISPLAYORIENTATION_LANDSCAPELEFT,
	DISPLAYORIENTATION_LANDSCAPERIGHT,
	DISPLAYORIENTATION_PORTRAIT
};

enum PresentInterval {
	/* For the default presentation interval, we try to use an OS-provided
	 * feature (if available) to sync when meeting the target framerate
	 * while tearing if the program misses vblank.
	 */
	PRESENTINTERVAL_DEFAULT,
	PRESENTINTERVAL_ONE,
	PRESENTINTERVAL_TWO,
	PRESENTINTERVAL_IMMEDIATE
};
	
struct Size2D
{
	int32_t Width;
	int32_t Height;
};
	
struct PresentationParameters {
	
	Size2D BackBufferSize;
	Size2D ViewPortBufferSize;
	
	int32_t multiSampleCount;
	void* deviceWindowHandle1;
	void* deviceWindowHandle2;
	uint8_t isFullScreen;
	DepthFormat depthStencilFormat;
	PresentInterval presentationInterval;
	//DisplayOrientation displayOrientation;
	//RenderTargetUsage renderTargetUsage;
};

enum ClearOptions {
	CLEAROPTIONS_TARGET = 1,
	CLEAROPTIONS_DEPTHBUFFER = 2,
	CLEAROPTIONS_STENCIL = 4
};


struct RenderTargetBinding {

	/* If this is >1, you MUST call ResolveTarget after rendering! */
	int32_t levelCount;

	/* If this is >1, colorBuffer MUST be non-NULL! */
	int32_t multiSampleCount;

	/* Destination texture. This MUST be non-NULL! */
	Texture* texture;

	/* If this is non-NULL, you MUST call ResolveTarget after rendering! */
	Renderbuffer* colorBuffer;

	Viewport viewport;
};



enum SurfaceFormat {
	SURFACEFORMAT_COLOR,
	SURFACEFORMAT_BGR565,
	SURFACEFORMAT_BGRA5551,
	SURFACEFORMAT_BGRA4444,
	SURFACEFORMAT_DXT1,
	SURFACEFORMAT_DXT3,
	SURFACEFORMAT_DXT5,
	SURFACEFORMAT_NORMALIZEDBYTE2,
	SURFACEFORMAT_NORMALIZEDBYTE4,
	SURFACEFORMAT_RGBA1010102,
	SURFACEFORMAT_RG32,
	SURFACEFORMAT_RGBA64,
	SURFACEFORMAT_ALPHA8,
	SURFACEFORMAT_SINGLE,
	SURFACEFORMAT_VECTOR2,
	SURFACEFORMAT_UINT,
	SURFACEFORMAT_VECTOR4,
	SURFACEFORMAT_HALFSINGLE,
	SURFACEFORMAT_HALFVECTOR2,
	SURFACEFORMAT_HALFVECTOR4,
	SURFACEFORMAT_HDRBLENDABLE,
	SURFACEFORMAT_COLORBGRA_EXT,
	SURFACEFORMAT_COLORSRGB_EXT,
	SURFACEFORMAT_DXT5SRGB_EXT,
	SURFACEFORMAT_BC7_EXT,
	SURFACEFORMAT_BC7SRGB_EXT,
	SURFACEFORMAT_VECTOR3,
};





};