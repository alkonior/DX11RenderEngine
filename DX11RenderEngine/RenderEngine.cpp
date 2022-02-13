#include "RenderEngine.h"
#include "pch.h"
#include "RenderEngine.h"
#include "PixelShader2D.h"
#include "VertexShader2D.h"

void RenderDevice::InitDevice(HWND hWnd, size_t width, size_t height) {
	if (gfx != nullptr) DestroyDevice();
	gfx = new Graphics(hWnd, width, height);
	gfx->SetCameraPosition({ float3(0,0,0), float3(0,0,0), float3(1,1,1) });


	ImGui_ImplWin32_Init(hWnd);

}

void RenderDevice::InitShaders(LPCWSTR dirr) {
	gfx->manager2D.Init(dirr);
	gfx->manager3D.Init(dirr);
	//PixelShader2D::Init(*gfx, dirr);
	//VertexShader2D::Init(*gfx, dirr);
}

void RenderDevice::ReloadShaders(LPCWSTR dirr) {
	//PixelShader2D::Release();
	//VertexShader2D::Release();
	//
	//PixelShader2D::Init(*gfx, dirr);
	//VertexShader2D::Init(*gfx, dirr);
}

void RenderDevice::ReloadShader(ShaderData shD) {
	switch (shD.type) {
	case ShaderType::Shader2D:
	{
		//PixelShader2D::Release();
		gfx->manager2D.Init(shD.data, shD.dataSize);
		//PixelShader2D::Init(*gfx, shD.data, shD.dataSize);
		break;
	}
	case ShaderType::Shader3D:
	{
		//VertexShader2D::Release();
		gfx->manager3D.Init(shD.data, shD.dataSize);
		//VertexShader2D::Init(*gfx, shD.data, shD.dataSize);
		break;
	}

	default:
		break;
	}
}

void RenderDevice::OnResize(size_t width, size_t height) {
	gfx->OnResize(width, height);
}

void RenderDevice::BeginFrame() {
	gfx->BeginFrame();
}

void RenderDevice::SetPerspectiveMatrix(matrix m) {
	gfx->SetCameraProjection(m);
}

void RenderDevice::SetViewMatrix(matrix m) {
	gfx->SetCameraMatrix(m);
}

void RenderDevice::Present() {
	gfx->EndFrame();
}

void RenderDevice::Clear(float r, float g, float b) {
	gfx->ClearBuffer(sm::Vector4(r, g, b, 1));
}

void RenderDevice::RegisterTexture(size_t id, LPCWCH file) {

	gfx->RegisterImg(id, TextureData::FromFile(file));
}

void RenderDevice::RegisterTexture(size_t id, const char* file) {

	gfx->RegisterImg(id, TextureData::FromFile(file));
}

void RenderDevice::RegisterTexture(size_t id, const TextureData& tex) {
	gfx->RegisterImg(id, tex);
}

void RenderDevice::UpdateTexture(size_t id, const TextureData& tex) {
	gfx->UpdateImg(id, tex);
}

void RenderDevice::RegisterModel(size_t id, const ModelData& model) {
	gfx->RegisterModel(id, model);
}

void RenderDevice::RegisterFramedModel(size_t id, const FramedModelData& model) {
	gfx->RegisterFramedModel(id, model);
}

void RenderDevice::ReleaseTexture(size_t id) {
	gfx->ReleaseImg(id);
}

void RenderDevice::DrawColor(float4 color, size_t x, size_t y, size_t width, size_t height, uint32_t flags) {
	gfx->DrawColor(color, x, y, width, height, flags|UICOLORED);
}

void RenderDevice::DrawImg(size_t id, size_t x, size_t y, size_t width, size_t height, uint32_t flags) {
	gfx->DrawImg(id, x, y, width, height, flags);
}

void RenderDevice::DrawImg(size_t id, size_t top, size_t left, size_t texW, size_t texH, size_t x, size_t y, size_t width, size_t height, uint32_t flags) {
	gfx->DrawImg(id, top, left, texW, texH, x, y, width, height, flags);
}

void RenderDevice::DrawModel(size_t modelId, size_t textureId, Transform position, size_t flags)
{
	gfx->DrawModel(modelId, textureId, position, flags);
}

void RenderDevice::DrawFramedModel(size_t modelId, size_t textureId, Transform position, int curIndex, int nextIndex, float alpha, size_t flags) {
	gfx->DrawFramedModel(modelId, textureId, position, curIndex, nextIndex, alpha, flags);
}

bool RenderDevice::ProcessMessages(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) {
		return true;
	}
	return false;
}

void RenderDevice::DestroyDevice() {

	//PixelShader2D::Release();
	//VertexShader2D::Release();
	ImGui_ImplWin32_Shutdown();
	if (!gfx) delete gfx;
	gfx = nullptr;
}
