#pragma once
#include "pch.h"
#include "SimpleMath.h"
#include "TexturesManager.h"
#include "ModelsManager.h"
#include "GraphicsBase.h"
#include "Geometry.h"

#include "UIRenderer.h"
#include "ModelRenderer.h"
#include "ImGUIRenderer.h"
#include "UPRenderer.h"


#ifdef _DEBUG
#include "DxgiInfoManager.h"
#endif

namespace sm = DirectX::SimpleMath;

class Graphics:public GraphicsBase{
public:

	Graphics(HWND hWnd, size_t width, size_t height);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	~Graphics() = default;
	void BeginFrame();
	void EndFrame();
	void ClearBuffer(sm::Vector4 color) noexcept;



	void RegisterImg(size_t id, const TextureData& text);
	void UpdateImg(size_t id, const TextureData& text);
	void ReleaseImg(size_t id);

	void DrawImg(size_t id, size_t x, size_t y, size_t width, size_t height, uint32_t flags);
	void DrawColor(float4 color, size_t x, size_t y, size_t width, size_t height, uint32_t flags);
	void DrawImg(size_t id, size_t top, size_t left, size_t texW, size_t texH, size_t x, size_t y, size_t width, size_t height, uint32_t flags);



	void RegisterModel(size_t id, const ModelData& model);
	void RegisterFramedModel(size_t id, const FramedModelData& model);
	void ReleaseModel(size_t id);




	void DrawModel(size_t modelId, size_t textureId, Transform position, size_t flags);
	void DrawUserPolygon(UPModelData model, size_t textureId, Transform position, size_t flags);
	void DrawFramedModel(size_t modelId, size_t textureId, Transform position, int curIndex, int nextIndex, float alpha, size_t flags);

	TexturesManager texturesManger;
	ModelsManager modelsManadger;

	UIRenderer manager2D;
	ModelRenderer manager3D;
	UPRenderer managerUP;
	ImGUIRenderer managerImGUI;

private:

	enum class DrawCall {
		draw2D,
		draw3D,
		drawUP,
	};

	std::vector<DrawCall> queue;

};