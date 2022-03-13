#include "RenderEngine.h"
#include "pch.h"
#include "RenderEngine.h"
#include "PixelShader2D.h"
#include "VertexShader2D.h"

void RenderDevice::InitDevice(HWND hWnd, size_t width, size_t height) {
	if (gfx != nullptr) DestroyDevice();
	gfx = new Graphics(hWnd, width, height);
	gfx->skyFlags = SKYNOTHING;
	//gfx->SetCameraPosition({ float3(0,0,0), float3(0,0,0), float3(1,1,1) });


	//ImGui_ImplWin32_Init(hWnd);

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
	case ShaderType::UIShader:
	{
		gfx->manager2D.Init(shD.data, shD.dataSize);
		break;
	}
	case ShaderType::ModelsShader:
	{
		gfx->manager3D.Init(shD.data, shD.dataSize);
		break;
	}
	case ShaderType::UPShader:
	{
		gfx->managerUP.Init(shD.data, shD.dataSize);
		break;
	}
	case ShaderType::ParticlesShader:
	{
		gfx->managerParticles.Init(shD.data, shD.dataSize);
		break;
	}
	case ShaderType::SkyShader:
	{
		gfx->managerSkybox.Init(shD.data, shD.dataSize);
		break;
	}
	case ShaderType::EndBSPShader:
	{
		gfx->managerEndUP.Init(shD.data, shD.dataSize);
		break;
	}
	case ShaderType::BloomShader:
	{
		gfx->managerBloom.Init(shD.data, shD.dataSize);
		break;
	}

	case ShaderType::FXAAShader:
	{
		gfx->managerFXAA.Init(shD.data, shD.dataSize);
		break;
	}

	default:
		break;
	}
}


void RenderDevice::BeginFrame() {
	gfx->BeginFrame();
}

void RenderDevice::SetRenderData(const RenderData& data){
	gfx->SetRenderData(data);
}

bool RenderDevice::Present() {
	return gfx->RenderFrame();
}

void RenderDevice::EndFrame()
{
	gfx->EndFrame();
}

void RenderDevice::SetSky(size_t side, const TextureData& data) {
	gfx->managerSkybox.UpdateSkybox(data, side);
}

void RenderDevice::SetSkyFlags(uint64_t flags) {
	gfx->skyFlags = flags;
}

void RenderDevice::Flush() {
	gfx->Flush();
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

void RenderDevice::RegisterTexture(size_t id, int width, int height, void* data, bool mipmap) {
	gfx->RegisterImg(id, width, height, data, mipmap);
}

void RenderDevice::UpdateTexture(size_t id, const TextureData& tex) {
	gfx->UpdateImg(id, tex);
}

void RenderDevice::UpdateTexture(const ImageUpdate& data) {
	gfx->UpdateImg(data);
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

void RenderDevice::DrawColor(const UIDrawData& data) {
	UIDrawData buff(data);
	buff.flag |= UICOLORED;
	gfx->DrawColor(buff);
}

void RenderDevice::DrawImg(size_t texId, const UIDrawData& data) {
	gfx->DrawImg(texId, data);
}

void RenderDevice::DrawModel(size_t modelId, size_t textureId, Transform position, size_t flags)
{
	gfx->DrawModel(modelId, textureId, position, flags);
}

void RenderDevice::DrawFramedModel(size_t modelId, size_t textureId, const LerpModelDrawData& data) {
	gfx->DrawFramedModel(modelId, textureId, data);
}

void RenderDevice::DrawUserPolygon(MeshHashData model, size_t textureId, UPDrawData data) {
	gfx->DrawUserPolygon(model, textureId, data);
}

void RenderDevice::DrawUserPolygon(MeshHashData model, size_t textureId, size_t lightmapId, UPDrawData data) {
	gfx->DrawUserPolygon(model, textureId, lightmapId, data);
}

void RenderDevice::DrawSetUserPolygon(MeshHashData model, UPModelData newModel, size_t textureId, UPDrawData data) {
	gfx->DrawSetUserPolygon(model,newModel, textureId, data);
}

MeshHashData RenderDevice::RegisterUserPolygon(UPModelData model, bool dynamic) {
	return gfx->RegisterhUserPolygon(model, dynamic);
}

void RenderDevice::DrawParticles(const ParticlesMesh& particles, const ParticlesDrawData& data) {
	gfx->DrawParticles(particles, data);
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
	//ImGui_ImplWin32_Shutdown();
	if (gfx) delete gfx;
	gfx = nullptr;
}
