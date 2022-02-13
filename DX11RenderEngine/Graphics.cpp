#include "pch.h"
#include "Graphics.h"
#include <sstream>
#include <cmath>
#include <d3dcompiler.h>
#include <filesystem>
#include <resource.h>

#include "imgui/imgui_impl_dx11.h"

#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"D3DCompiler.lib")


using namespace Renderer;

Graphics::Graphics(HWND hWnd, size_t width, size_t height)
	:GraphicsBase(hWnd, width, height), manager2D(&renderer), manager3D(&renderer), modelsManadger(&renderer), texturesManger(&renderer) {
	managerImGUI.Init();
	ImGui_ImplDX11_Init(renderer.device.Get(), renderer.context.Get());

}

void Graphics::BeginFrame() {
	managerImGUI.BeginFrame(*this);

	manager3D.Clear();
	manager2D.Clear();
}



void Graphics::EndFrame() {

	GFX_CATCH_RENDER(manager3D.Render(*this););
	GFX_CATCH_RENDER(manager2D.Render(););
	GFX_CATCH_RENDER(managerImGUI.Render(););

	renderer.SwapBuffers();
	//
	//
	//#ifndef NDEBUG
	//	infoManager.Set();
	//#endif
	//	if (FAILED(hr = pSwap->Present(1u, 0u))) {
	//		if (hr == DXGI_ERROR_DEVICE_REMOVED) {
	//			throw GFX_DEVICE_REMOVED_EXCEPT(pDevice->GetDeviceRemovedReason());
	//		} else {
	//			throw GFX_EXCEPT(hr);
	//		}
	//	}
}

void Graphics::ClearBuffer(sm::Vector4 color) noexcept {
	renderer.Clear((ClearOptions)7, { color.x, color.y, color.z, color.w }, 1, 0u);
	//pContext->ClearRenderTargetView(pTarget.Get(), reinterpret_cast<float*>(&color));
	//pContext->ClearDepthStencilView(pDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
}



void Graphics::DrawImg(size_t id, size_t x, size_t y, size_t width, size_t height, uint32_t flags) {
	manager2D.Draw(texturesManger.GetImg(id), x, y, width, height, flags);
}


void Graphics::DrawImg(size_t id, size_t top, size_t left, size_t texW, size_t texH, size_t x, size_t y, size_t width, size_t height, uint32_t flags) {
	manager2D.Draw(texturesManger.GetImg(id), top, left, texW, texH, x, y, width, height, flags);
}

void Graphics::DrawColor(float4 color, size_t x, size_t y, size_t width, size_t height, uint32_t flags) {
	manager2D.Draw(color, x, y, width, height, flags);
}

void Graphics::RegisterImg(size_t id, const TextureData& text) {
	texturesManger.RegTexture(text, id);
}

void Graphics::UpdateImg(size_t id, const TextureData& text) {
	texturesManger.UpdateTexture(text, id);
}

void Graphics::ReleaseImg(size_t id) {
	texturesManger.ReleaseTexture(id);
}

void Graphics::RegisterModel(size_t id, const ModelData& model) {
	modelsManadger.RegisterModel(model, id);
}

void Graphics::RegisterFramedModel(size_t id, const FramedModelData& model) {
	modelsManadger.RegisterFramedModel(model, id);
}

void Graphics::ReleaseModel(size_t id) {
	modelsManadger.ReleaseModel(id);
}

void Graphics::DrawModel(size_t modelId, size_t textureId, Transform position, size_t flags) {
	manager3D.Draw(modelsManadger.GetModel(modelId), texturesManger.GetImg(textureId), position, flags);
}

void Graphics::DrawFramedModel(size_t modelId, size_t textureId, Transform position, int curIndex, int nextIndex, float alpha, size_t flags) {
	manager3D.DrawLerp(modelsManadger.GetModel(modelId), texturesManger.GetImg(textureId), position, curIndex, nextIndex, alpha, flags);
}


