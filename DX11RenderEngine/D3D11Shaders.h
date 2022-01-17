#pragma once
#include "pch.h"
#include "Utils.h"
#include "SimpleMath.h"

namespace Renderer {

//struct Vertex2D {
//	DirectX::SimpleMath::Vector2 pos;
//	DirectX::SimpleMath::Vector2 uv;
//};
//
//struct VertexShaderCosntBuffer {
//	DirectX::SimpleMath::Matrix transform;
//	DirectX::SimpleMath::Vector2 uvShift;
//	DirectX::SimpleMath::Vector2 uvScale;
//};

struct D3D11VertexShader: public VertexShader {
	//using mat = DirectX::SimpleMath::Matrix;
	//using vec2 = DirectX::SimpleMath::Vector2;
	wrl::ComPtr<ID3D11VertexShader> pVertexShader;
	wrl::ComPtr<ID3D11InputLayout> pInputLayout;
	//wrl::ComPtr<ID3D10Blob> pVSData = nullptr;

	//VertexShaderCosntBuffer localBuffer;
	//wrl::ComPtr<ID3D11Buffer> pConstantBuffer;
	//void SetTransform(mat transform, vec2 uvShift, vec2 uvScale) {
	//	localBuffer.transform = transform.Transpose();
	//	localBuffer.uvShift = uvShift;
	//	localBuffer.uvScale = uvScale;
	//};

};

struct  D3D11PixelShader: public PixelShader {
	wrl::ComPtr<ID3D11PixelShader> pPixelShader = nullptr;
	//wrl::ComPtr<ID3D10Blob> pPSData = nullptr;
};


};

