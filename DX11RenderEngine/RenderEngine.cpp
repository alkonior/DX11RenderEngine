#include "pch.h"
#include "RenderEngine.h"
#include "PixelShader2D.h"
#include "VertexShader2D.h"

void RenderDevice::InitDevice(HWND hWnd, size_t width, size_t height) {
	if (gfx!= nullptr) DestroyDevice();
	gfx = new Graphics(hWnd, width, height);
}

void RenderDevice::InitShaders(LPCWSTR dirr) {
	PixelShader2D::Init(*gfx, dirr);
	VertexShader2D::Init(*gfx, dirr);
}

void RenderDevice::ReloadShaders(LPCWSTR dirr) {
	PixelShader2D::Release();
	VertexShader2D::Release();

	PixelShader2D::Init(*gfx, dirr);
	VertexShader2D::Init(*gfx, dirr);
}

void RenderDevice::ReloadShader(ShaderData shD) {
	switch (shD.type) {
	case ShaderType::PixelShader2D :{
			PixelShader2D::Release();
			PixelShader2D::Init(*gfx, shD.data, shD.dataSize);
		}
	default:
		break;
	}
}

void RenderDevice::Present() {
	gfx->EndFrame();
}

void RenderDevice::Clear(float r, float g, float b) {
	gfx->ClearBuffer(sm::Vector4(r,g,b,1));
}

void RenderDevice::RegisterTexture(const char* name, LPCWCH file) {

	gfx->RegisterImg(name, Texture::FromFile(file));
}

void RenderDevice::RegisterTexture(const char* name, const char* file) {

	gfx->RegisterImg(name, Texture::FromFile(file));
}

void RenderDevice::RegisterTexture(const char* name, const Texture& tex) {
	gfx->RegisterImg(name, tex);
}

void RenderDevice::DrawImg(const char* name, size_t x, size_t y, size_t width, size_t height) {
	gfx->DrawImg(name, x, y, width, height);
}

void RenderDevice::DrawImg(const char* name, size_t top, size_t left, size_t texW, size_t texH, size_t x, size_t y, size_t width, size_t height) {
	gfx->DrawImg(name, top, left, texW, texH, x, y, width, height);
}

void RenderDevice::DestroyDevice() {

	PixelShader2D::Release();
	VertexShader2D::Release();
	if (!gfx) delete gfx;
	gfx = nullptr;
}
