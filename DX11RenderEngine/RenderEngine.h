#pragma once
#include "CoreRenderSystem/Graphics.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

struct RenderDevice {

	Graphics* gfx = nullptr;

	/* Init */

	void InitDevice(HWND hWnd, size_t width, size_t height);

	/* Quit */

	void DestroyDevice();


	enum class ShaderType {
		UIShader,
		ModelsShader,
		UPShader,
		ParticlesShader,
		SkyShader,
		EndBSPShader,
		BloomShader,
		FXAAShader,
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


	void BeginFrame();
	void SetRenderData(const RenderData& data);
	bool Present();
	void EndFrame();
	void Clear(float r, float g, float b);

	/* Resourse loading*/
#ifdef WIN32
	void RegisterTexture(size_t id, LPCWCH file);
#endif
	void RegisterTexture(size_t id, const char* file);
	void RegisterTexture(size_t id, const TextureData&);
	void RegisterTexture(size_t id, int width, int height, void* data, bool mipmap);
	void UpdateTexture(size_t id, const TextureData&);
	void UpdateTexture(const ImageUpdate &);


	void RegisterModel(size_t id, const ModelData&);
	void RegisterFramedModel(size_t id, const FramedModelData&);
	void ReleaseTexture(size_t id);


	/* Drawing */

	void DrawColor(const UIDrawData& data);
	void DrawImg(size_t texId, const UIDrawData& data);
	
	
	
	void DrawModel(size_t modelId, size_t textureId, Transform position, size_t flags);
	void DrawFramedModel(size_t modelId, size_t textureId, const LerpModelDrawData& data);
	
	
	void DrawUserPolygon(MeshHashData model, size_t textureId, UPDrawData data);
	void DrawUserPolygon(MeshHashData model, size_t textureId, size_t lightmapId, UPDrawData data);
	void DrawSetUserPolygon(MeshHashData model, UPModelData newModel, size_t textureId, UPDrawData data);
	MeshHashData RegisterUserPolygon(UPModelData model, bool dynamic);

	void DrawParticles(const ParticlesMesh& particles, const ParticlesDrawData& data);
	


	void SetSky(size_t side, const TextureData& data);
	void SetSkyFlags(uint64_t flags);

	void Flush();
	/* Debug staff */

	bool ProcessMessages(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

};






