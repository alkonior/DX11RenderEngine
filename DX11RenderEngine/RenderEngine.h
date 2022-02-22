#pragma once
#include "Graphics.h"
#include "SimpleMath.h"
#include "TextureData.h"


#include "imgui/imgui_impl_win32.h"

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

	void OnResize(size_t width, size_t height);

	void BeginFrame();
	void SetPerspectiveMatrix(matrix m);
	void SetViewMatrix(matrix m);
	bool Present();
	void Clear(float r, float g, float b);

	/* Resourse loading*/
#ifdef WIN32
	void RegisterTexture(size_t id, LPCWCH file);
#endif
	void RegisterTexture(size_t id, const char* file);
	void RegisterTexture(size_t id, const TextureData&);
	void RegisterTexture(size_t id, int width, int height, void* data, bool mipmap);
	void UpdateTexture(size_t id, const TextureData&);
	void UpdateTexture(size_t id, int32_t x, int32_t y, int32_t w, int32_t h, int32_t level, void* data);
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
	

	void Flush();
	/* Debug staff */

	bool ProcessMessages(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

};






