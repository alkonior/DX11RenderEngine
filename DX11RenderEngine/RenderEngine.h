#pragma once
#include "Graphics.h"
#include "SimpleMath.h"

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

	void Present();
	void Clear(float r, float g, float b);

	/* Resourse loading*/



	/* Drawing */




};






