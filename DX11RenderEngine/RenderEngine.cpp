#include "pch.h"
#include "RenderEngine.h"
#include "DefaultPixelShader.h"
#include "DefaultVertexShader.h"

void RenderDevice::InitDevice(HWND hWnd) {
	if (gfx!= nullptr) DestroyDevice();
	gfx = new Graphics(hWnd);

	DefaultPixelShader::Init(*gfx);
	DefaultVertexShader::Init(*gfx);
}

void RenderDevice::Present() {
	gfx->DrawTestTriangle(0, 1 , 1);
	gfx->EndFrame();
}

void RenderDevice::Clear(float r, float g, float b) {
	gfx->ClearBuffer(sm::Vector4(r,g,b,1));
}

void RenderDevice::DestroyDevice() {

	DefaultPixelShader::Release();
	DefaultVertexShader::Release();
	if (!gfx) delete gfx;
	gfx = nullptr;
}
