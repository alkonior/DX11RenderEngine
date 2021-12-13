#pragma once
#include "pch.h"
#include "Drawable.h"
#include "PixelShader2D.h"
#include "VertexShader2D.h"


class Manager2D : public Drawable {

	struct Rectangle {
		Rectangle(wrl::ComPtr<ID3D11Texture2D> texture, size_t x, size_t y, size_t width, size_t height);

		dx::SimpleMath::Matrix getTransform(size_t screenW, size_t screenH);

		size_t x; size_t y; size_t width; size_t height;
		wrl::ComPtr<ID3D11Texture2D> texture;
	};

public:
	Manager2D(GraphicsBase&);

	void Draw(wrl::ComPtr<ID3D11Texture2D> texture, size_t x, size_t y, size_t width, size_t height);

	virtual void Present(GraphicsBase&) override;

	~Manager2D() override;


private:

	wrl::ComPtr<ID3D11Buffer> pVertexBuffer;
	wrl::ComPtr<ID3D11Buffer> pIndexBuffer;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> pSampler;
	std::vector<Rectangle> drawCalls;
};

