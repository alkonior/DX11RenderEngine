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
	void Present();
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

	void DrawImg(size_t id, size_t x, size_t y, size_t width, size_t height, uint32_t flags);
	void DrawColor(float4 color, size_t x, size_t y, size_t width, size_t height, uint32_t flags);
	void DrawImg(size_t id, size_t top, size_t left, size_t texW, size_t texH, size_t x, size_t y, size_t width, size_t height, uint32_t flags);
	
	
	
	void DrawModel(size_t modelId, size_t textureId, Transform position, size_t flags);
	void DrawFramedModel(size_t modelId, size_t textureId, const LerpModelDrawData& data);
	
	
	void DrawUserPolygon(UPHashData model, size_t textureId, UPDrawData data);
	void DrawUserPolygon(UPHashData model, size_t textureId, size_t lightmapId, UPDrawData data);
	void DrawSetUserPolygon(UPHashData model, UPModelData newModel, size_t textureId, UPDrawData data);
	UPHashData RegisterUserPolygon(UPModelData model);
	

	/* Debug staff */

	bool ProcessMessages(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

};






