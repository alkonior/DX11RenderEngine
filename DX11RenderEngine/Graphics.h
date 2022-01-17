#pragma once
#include "pch.h"
#include "GraphicsException.h"
#include "TexturesManager.h"
#include "GraphicsBase.h"
#include "Geometry.h"
#include "UIRenderer.h"
#include "SimpleMath.h"


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



	void DrawImg(size_t id, size_t x, size_t y, size_t width, size_t height, uint32_t flags);
	void DrawImg(size_t id, size_t top, size_t left, size_t texW, size_t texH, size_t x, size_t y, size_t width, size_t height, uint32_t flags);
	void RegisterImg(size_t id, const TextureData& text);
	void ReleaseImg(size_t id);

	TexturesManager texturesManger;
	UIRenderer manager2D;
};