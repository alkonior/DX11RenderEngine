#pragma once
#include <cstdint>



namespace Renderer {

struct Texture {};
struct Buffer {};
struct ConstBuffer {};
struct Renderbuffer {};
struct VertexBufferBinding {
	size_t buffersCount = 1;
	Buffer** vertexBuffers;
	unsigned int* vertexStride;
	unsigned int* vertexOffset;
};

struct ShaderDefines {
	const char* name;
	const char* defenition;

};
struct PixelShader {};
struct VertexShader {};
//struct ComputeShader {};


struct Color {
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
	inline bool operator!= (const Color& rhs) { return ((r != rhs.r) || (g != rhs.g) || (b != rhs.b) || (a != rhs.a));}
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
	PRIMITIVETYPE_POINTLIST_EXT
};

enum DepthFormat {
	DEPTHFORMAT_NONE,
	DEPTHFORMAT_D16,
	DEPTHFORMAT_D24,
	DEPTHFORMAT_D24S8
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

struct PresentationParameters {
	int32_t backBufferWidth;
	int32_t backBufferHeight;
	int32_t multiSampleCount;
	void* deviceWindowHandle;
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
};






};