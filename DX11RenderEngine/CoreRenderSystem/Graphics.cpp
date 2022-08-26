#include "pch.h"
#include "Graphics.h"
#include <sstream>
#include <cmath>
#include <d3dcompiler.h>
#include <filesystem>
#include <resource.h>


#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"

#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"D3DCompiler.lib")


using namespace Renderer;

Graphics::Graphics(InitStruct init)
	:GraphicsBase(init)
{
	ImGui::CreateContext();
	
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange | ImGuiConfigFlags_ViewportsEnable;
	
	ImGui_ImplWin32_Init(init.hWnd1);
	ImGui_ImplDX11_Init(pRenderer.device.Get(), pRenderer.context.Get());
	
	//managerImGUI.Init();
	//ImGui_ImplDX11_Init(renderer.device.Get(), renderer.context.Get());7

	pLocalConstants = renderer->CreateConstBuffer(sizeof(taaConstants));
	taaConstants.taaStrength = 1;
	renderer->VerifyConstBuffer(pLocalConstants, taaShiftBuffer.slot);

	renderPasses.push_back(&managerSkybox);
	renderPasses.push_back(&managerUI);
	renderPasses.push_back(&managerModels);
	renderPasses.push_back(&managerMB);
	renderPasses.push_back(&managerUP);
	renderPasses.push_back(&managerPostProcess);
	renderPasses.push_back(&managerParticles);
	renderPasses.push_back(&managerBloom);
	renderPasses.push_back(&managerFXAA);
	renderPasses.push_back(&managerTAA);
	renderPasses.push_back(&managerSSAO);

}

void Graphics::ReinitShaders(const char * dirr)
{
	for(auto pass : renderPasses)
	{
		pass->Init(dirr);
	}
};

void Graphics::BeginFrame() {
	for(auto pass : renderPasses)
	{
		pass->Clear(*this);
	}
}

//#define GFX_CATCH_RENDER(render) try {render} catch (const std::exception& exe) {printf(exe.what()); printf("\n"); static char c[100]; scanf("%s", c); success = false; }
#define GFX_CATCH_RENDER(render) {render}

bool Graphics::RenderFrame() {
	managerIMGUI.BeginFrame(*this);

	renderer->ClearState();
	
	managerTAA.UpdateHaltonSequence();
	taaConstants.taaPixelShift = managerTAA.HaltonSequence[managerTAA.HaltonIndex];
	renderer->SetConstBuffer(pLocalConstants, &taaConstants);
	renderer->VerifyConstBuffer(pLocalConstants, taaShiftBuffer.slot);
	renderer->VerifyConstBuffer(GraphicsBase::pLocalConstants, mainConstants.slot);
	
	bool success = true;
	pRenderer.BeginEvent("BSP draw.");
	GFX_CATCH_RENDER(managerUP.Render(*this););
	pRenderer.EndEvent();
	
	renderer->ClearState();
	
	pRenderer.BeginEvent("BloomMask draw.");
	//GFX_CATCH_RENDER(managerBloom.RenderBloomMask(*this););
	pRenderer.EndEvent();
	
	pRenderer.BeginEvent("Static motion blur draw.");
	//GFX_CATCH_RENDER(managerMB.RenderStatic(*this););
	pRenderer.EndEvent();
	
	pRenderer.BeginEvent("Models draw.");
	//GFX_CATCH_RENDER(managerModels.Render(*this););
	pRenderer.EndEvent();

	pRenderer.BeginEvent("Dynamic motion blur draw.");
	//GFX_CATCH_RENDER(managerMB.RenderDynamic(*this););
	pRenderer.EndEvent();
	
	pRenderer.BeginEvent("SSAO draw.");
	//GFX_CATCH_RENDER(managerSSAO.Render(*this););
	pRenderer.EndEvent();
	
	pRenderer.BeginEvent("Sky draw.");
	//GFX_CATCH_RENDER(managerSkybox.Render(*this););
	pRenderer.EndEvent();

	pRenderer.BeginEvent("Particles draw.");
	//GFX_CATCH_RENDER(managerParticles.Render(*this););
	pRenderer.EndEvent();

	pRenderer.BeginEvent("Bloom pass.");
	//GFX_CATCH_RENDER(managerBloom.Render(*this););
	pRenderer.EndEvent();
	
	pRenderer.BeginEvent("End BSP draw.");
	//GFX_CATCH_RENDER(managerPostProcess.Render(*this););
	pRenderer.EndEvent();
	
	pRenderer.BeginEvent("TAA-pass.");
	//GFX_CATCH_RENDER(managerTAA.Render(*this););
	pRenderer.EndEvent();
	
	pRenderer.BeginEvent("UI draw.");
	GFX_CATCH_RENDER(managerUI.Render(););
	pRenderer.EndEvent();
	
	return success;
}

void Graphics::EndFrame()
{
	pRenderer.BeginEvent("IMGUI draw.");
	managerIMGUI.Render();
	pRenderer.EndEvent();
	pRenderer.SwapBuffers();
}

void Graphics::ClearBuffer(sm::Vector4 color) noexcept {
	pRenderer.SetRenderTargets(nullptr, 0, texturesManger.depthBuffer, Viewport());
	pRenderer.Clear((ClearOptions)7, { color.x, color.y, color.z, color.w }, 1, 0u);
	//pContext->ClearRenderTargetView(pTarget.Get(), reinterpret_cast<float*>(&color));
	//pContext->ClearDepthStencilView(pDSV.Get(), DModels11_CLEAR_DEPTH, 1.0f, 0u);
}

void Graphics::Flush() {
	managerUP.Flush();
}



void Graphics::DrawImg(size_t texId, const UIDrawData& data) {
	//queue.push_back(DrawCall::draw2D).
	managerUI.Draw(texturesManger.GetImg(texId), data);
}


void Graphics::DrawColor(const UIDrawData& data) {
	managerUI.Draw(data);
}

void Graphics::RegisterImg(size_t id, const TextureData& text) {
	texturesManger.RegTexture(text, id);
}

void Graphics::UpdateImg(size_t id, const TextureData& text) {
	texturesManger.UpdateTexture(text, id);
}

void Graphics::UpdateImg(const ImageUpdate& data) {
	texturesManger.UpdateTexture(data);
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

void Graphics::RegisterImg(size_t id, int width, int height, void* data, bool mipmap) {
	texturesManger.RegTexture(data, width, height, mipmap, id);

}

void Graphics::DrawModel(size_t modelId, size_t textureId, Transform position, size_t flags) {
	managerModels.Draw(modelsManadger.GetModel(modelId), texturesManger.GetImg(textureId), position, flags);
}

void Graphics::DrawUserPolygon(MeshHashData model, size_t textureId, UPDrawData data) {
	managerUP.Draw(model, texturesManger.GetImg(textureId), data);
}
void Graphics::DrawUserPolygon(MeshHashData model, size_t textureId, size_t lightmapId, UPDrawData data) {
	managerUP.Draw(model, texturesManger.GetImg(textureId), texturesManger.GetImg(lightmapId), data);
}

void Graphics::DrawSetUserPolygon(MeshHashData model, UPModelData newModel, size_t textureId, UPDrawData data) {
	managerUP.DrawSet(model, newModel, texturesManger.GetImg(textureId), data);
}

MeshHashData Graphics::RegisterhUserPolygon(UPModelData model, bool dynamic) {
	return managerUP.Register(model, dynamic);
}

void Graphics::DrawFramedModel(size_t modelId, size_t textureId, const LerpModelDrawData& data) {
	managerModels.DrawLerp(modelsManadger.GetModel(modelId), texturesManger.GetImg(textureId), data);
}

void Graphics::DrawParticles(const ParticlesMesh& particles, const ParticlesDrawData& data) {
	managerParticles.Draw(particles, data);
}


