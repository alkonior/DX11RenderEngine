#include "pch.h"
#include "RenderEngine.h"
#include "PixelShader2D.h"
#include "VertexShader2D.h"

void RenderDevice::InitDevice(HWND hWnd, size_t width, size_t height) {
	if (gfx!= nullptr) DestroyDevice();
	gfx = new Graphics(hWnd, width, height);

	auto testText = Texture::FromFile("Img\\kappa50.png");
	gfx->RegisterImg("test", testText);

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
	//gfx->DrawTestRectangle(  0,  0,   200, 200);
	//gfx->DrawTestRectangle( 50, 50,   200, 200);
	//gfx->DrawTestRectangle(100,100,   200, 200);
	//gfx->DrawTestRectangle(150,150,   200, 200);
	gfx->DrawImg("test", 0, 0, 200, 200);
	gfx->EndFrame();
}

void RenderDevice::Clear(float r, float g, float b) {
	gfx->ClearBuffer(sm::Vector4(r,g,b,1));
}

void RenderDevice::DestroyDevice() {

	PixelShader2D::Release();
	VertexShader2D::Release();
	if (!gfx) delete gfx;
	gfx = nullptr;
}
