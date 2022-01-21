#pragma once
#include "pch.h"
#include "D3D11Renderer.h"
#include "TexturesManager.h"
#include "PixelShader2D.h"
#include "VertexShader2D.h"


class Manager2D {
	struct VertexShaderCosntBuffer {
		DirectX::SimpleMath::Matrix transform;
		DirectX::SimpleMath::Vector2 uvShift;
		DirectX::SimpleMath::Vector2 uvScale;
	};

	struct DrawCall {
		DrawCall(TexturesManager::TextureCache texture, size_t top, size_t left, size_t texW, size_t texH, size_t x, size_t y, size_t width, size_t height);
		DrawCall(TexturesManager::TextureCache texture, size_t x, size_t y, size_t width, size_t height);
		
		dx::SimpleMath::Matrix getTransform(size_t screenW, size_t screenH);
		dx::SimpleMath::Vector2 getUVShift();
		dx::SimpleMath::Vector2 getUVScale();

		size_t x; size_t y; size_t width; size_t height; size_t top; size_t left; size_t texW; size_t texH;
		TexturesManager::TextureCache texture;
	};

public:
	Manager2D(GraphicsBase&);

	void CompileShaders(GraphicsBase& gfx,LPCWSTR dirr);
	void CompileShaders(GraphicsBase& gfx,BYTE* shaderData, size_t size);
	Renderer::VertexShader* vertexShader;
	Renderer::PixelShader* pixelShader;




	void Draw(TexturesManager::TextureCache texture, size_t top, size_t left, size_t texW, size_t texH, size_t x, size_t y, size_t width, size_t height);
	void Draw(TexturesManager::TextureCache texture, size_t x, size_t y, size_t width, size_t height);

	void Present(GraphicsBase&);

	~Manager2D();


private:
	Renderer::Viewport vp;
	Renderer::D3D11VertexBufferBinding vertexBuffer;
	Renderer::Buffer* indexBuffer;

	VertexShaderCosntBuffer localBuffer;
	Renderer::ConstBuffer* constBuffer;
	Renderer::SamplerState sampler;
	//wrl::ComPtr<ID3D11Buffer> pVertexBuffer;
	//wrl::ComPtr<ID3D11Buffer> pIndexBuffer;
	//Microsoft::WRL::ComPtr<ID3D11SamplerState> pSampler;
	std::vector<DrawCall> drawCalls;
};

