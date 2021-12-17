#pragma once
#include "Graphics.h"
#include "SimpleMath.h"
#include "Texture.h"

struct RenderDevice {

	Graphics* gfx = nullptr;

	/* Init */

	void DestroyDevice();

	/* Quit */

	void InitDevice(HWND hWnd, size_t width, size_t height);

	enum ShaderType {
		VertexShader2D,
		PixelShader2D,
	};
	struct ShaderData {
		ShaderType type;
		BYTE* data;
		size_t dataSize;
	};

	void InitShaders(LPCWSTR);
	void ReloadShaders(LPCWSTR);
	void ReloadShader(ShaderData shD);

	/* Presentation */

	void OnResize(size_t width, size_t height);
	void Present();
	void Clear(float r, float g, float b);

	/* Resourse loading*/
#ifdef WIN32
	void RegisterTexture(size_t id, LPCWCH file);
#endif
	void RegisterTexture(size_t id, const char* file);
	void RegisterTexture(size_t id, const Texture&);


	/* Drawing */

	void DrawImg(size_t id, size_t x, size_t y, size_t width, size_t height);
	void DrawImg(size_t id, size_t top, size_t left, size_t texW, size_t texH, size_t x, size_t y, size_t width, size_t height);

};






