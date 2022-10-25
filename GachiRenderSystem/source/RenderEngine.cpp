#include "pch.h"
#include "RenderEngine.h"
#include "RenderSystem.h"


void RenderDevice::InitDevice(RenderEngineInitStruct init) {
	if (gfx != nullptr) DestroyDevice();
	gfx =  RenderSystem::Initialise(init);
	//gfx->skyFlags = SKYNOTHING;
	//gfx->SetCameraPosition({ float3(0,0,0), float3(0,0,0), float3(1,1,1) });


	//ImGui_ImplWin32_Init(hWnd);

}

void RenderDevice::InitShaders(const char* dirr) {
	shadersDir = dirr;
	gfx->ReinitShaders(dirr);
}
void RenderDevice::ReloadShaders()
{
	gfx->ReinitShaders(shadersDir.c_str());
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
	//gfx->managerSkybox.UpdateSkybox(data, side);
}

void RenderDevice::SetSkyFlags(uint64_t flags) {
	//gfx->skyFlags = flags;
}

void* RenderDevice::GetGameTexture()
{
	return gfx->pRenderer->GetNativeTexture(gfx->texturesManger->GetRenderTarget(SID("outTexture"))->texture);
}

void RenderDevice::Flush() {
	//gfx->PostRender();
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

void RenderDevice::DrawModel(const ModelDrawData& drawData)
{
	gfx->DrawModel(drawData);
}

void RenderDevice::DrawUserPolygon(MeshHashData model, size_t textureId, UPDrawData data) {
	gfx->DrawUserPolygon(model, textureId, data);
}

void RenderDevice::DrawUserPolygon(MeshHashData model, size_t textureId, size_t lightmapId, UPDrawData data) {
	gfx->DrawUserPolygon(model, textureId, lightmapId, data);
}

void RenderDevice::DrawSetUserPolygon(MeshHashData model, UPModelMesh newModel, size_t textureId, UPDrawData data) {
	gfx->DrawSetUserPolygon(model,newModel, textureId, data);
}

MeshHashData RenderDevice::RegisterUserPolygon(UPModelMesh model, bool dynamic) {
	return gfx->RegisterStaticPolygon(model, dynamic);
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
