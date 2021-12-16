#pragma once
#include "pch.h"
#include "Drawable.h"
#include "TexturesManager.h"
#include "PixelShader2D.h"
#include "VertexShader2D.h"


class Manager2D : public Drawable {

	struct Rectangle {
		Rectangle(TexturesManager::TextureCache texture, size_t top, size_t left, size_t texW, size_t texH, size_t x, size_t y, size_t width, size_t height);
		Rectangle(TexturesManager::TextureCache texture, size_t x, size_t y, size_t width, size_t height);
		
		dx::SimpleMath::Matrix getTransform(size_t screenW, size_t screenH);
		dx::SimpleMath::Vector2 getUVShift();
		dx::SimpleMath::Vector2 getUVScale();

		size_t x; size_t y; size_t width; size_t height; size_t top; size_t left; size_t texW; size_t texH;
		TexturesManager::TextureCache texture;
	};

public:
	Manager2D(GraphicsBase&);

	void Draw(TexturesManager::TextureCache texture, size_t top, size_t left, size_t texW, size_t texH, size_t x, size_t y, size_t width, size_t height);
	void Draw(TexturesManager::TextureCache texture, size_t x, size_t y, size_t width, size_t height);

	virtual void Present(GraphicsBase&) override;

	~Manager2D() override;


private:

	wrl::ComPtr<ID3D11Buffer> pVertexBuffer;
	wrl::ComPtr<ID3D11Buffer> pIndexBuffer;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> pSampler;
	std::vector<Rectangle> drawCalls;
};

