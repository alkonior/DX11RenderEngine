#pragma once
#include "Graphics.h"
#include "SimpleMath.h"

struct RenderDevice {

	Graphics* gfx = nullptr;

	/* Init */

	void DestroyDevice();

	/* Quit */

	void InitDevice(HWND hWnd);

	/* Presentation */

	void Present();
	void Clear(float r, float g, float b);

	/* Resourse loading*/



	/* Drawing */




};






