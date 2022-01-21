//#pragma once
//#include "pch.h"
//#include "GraphicsException.h"
//#include "GraphicsBase.h"
//#include "SimpleMath.h"
//
//#ifdef _DEBUG
//#include "DxgiInfoManager.h"
//#endif
//
//
//struct Vertex2D {
//	DirectX::SimpleMath::Vector2 pos;
//	DirectX::SimpleMath::Vector2 uv;
//};
//
//struct Shader2DCosntBuffer {
//	DirectX::SimpleMath::Matrix transform;
//	DirectX::SimpleMath::Vector2 uvShift;
//	DirectX::SimpleMath::Vector2 uvScale;
//};
//
//struct VertexShader2D {
//	using mat = DirectX::SimpleMath::Matrix;
//	using vec2 = DirectX::SimpleMath::Vector2;
//
//	static void Init(GraphicsBase&, LPCWSTR);
//	static void Init(GraphicsBase&, LPVOID data, size_t size);
//	static void Bind(GraphicsBase&);
//	static void BindViewport(GraphicsBase&, D3D11_VIEWPORT);
//	static void Release();
//
//
//	static wrl::ComPtr<ID3D11VertexShader> pVertexShader;
//	static const D3D11_INPUT_ELEMENT_DESC inputLayout[ ];
//	static wrl::ComPtr<ID3D11InputLayout> pInputLayout;
//
//
//	static Shader2DCosntBuffer localBuffer;
//	static wrl::ComPtr<ID3D11Buffer> pConstantBuffer;
//	static void SetTransform(mat proj, vec2 uvShift, vec2 uvScale);
//	static void UpdateConstBuff(GraphicsBase&);
//
//};