#pragma once
#include "pch.h"
#include "SimpleMath.h"
#include "TexturesManager.h"
#include "ModelsManager.h"
#include "GraphicsBase.h"
#include "Geometry.h"

#include "UIRenderer.h"
#include "ModelRenderer.h"


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
	void EndFrame();
	void ClearBuffer(sm::Vector4 color) noexcept;



	void RegisterImg(size_t id, const TextureData& text);
	void ReleaseImg(size_t id);

	void DrawImg(size_t id, size_t x, size_t y, size_t width, size_t height, uint32_t flags);
	void DrawImg(size_t id, size_t top, size_t left, size_t texW, size_t texH, size_t x, size_t y, size_t width, size_t height, uint32_t flags);



	void RegisterModel(size_t id, const ModelData& model);
	void ReleaseModel(size_t id);

	void DrawModel(size_t modelId, size_t textureId, Transform position, size_t flags);

	TexturesManager texturesManger;
	ModelsManager modelsManadger;

	UIRenderer manager2D;
	ModelRenderer manager3D;
};