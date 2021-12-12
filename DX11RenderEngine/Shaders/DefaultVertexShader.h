#pragma once
#include "pch.h"
#include "GraphicsException.h"
#include "GraphicsBase.h"
#include "SimpleMath.h"

#ifdef _DEBUG
#include "DxgiInfoManager.h"
#endif


struct Vertex {
	struct {
		float x;
		float y;
	} pos;
	struct {
		unsigned char r;
		unsigned char g;
		unsigned char b;
		unsigned char a;
	} color;
};

struct VertexShaderCosntBuffer {
	DirectX::SimpleMath::Matrix world;
	DirectX::SimpleMath::Matrix view;
	DirectX::SimpleMath::Matrix projection;
};

struct DefaultVertexShader {
	using mat = DirectX::SimpleMath::Matrix;

	static void Init(GraphicsBase&, LPCWSTR);
	static void Bind(GraphicsBase&);
	static void BindViewport(GraphicsBase&, D3D11_VIEWPORT);
	static void Release();


	static wrl::ComPtr<ID3D11VertexShader> pVertexShader;
	static BYTE* data;
	static size_t dataSize;
	static const D3D11_INPUT_ELEMENT_DESC inputLayout[ ];
	static wrl::ComPtr<ID3D11InputLayout> pInputLayout;


	static VertexShaderCosntBuffer localBuffer;
	static wrl::ComPtr<ID3D11Buffer> pConstantBuffer;
	static void SetWorld(mat world);
	static void SetView(mat view);
	static void SetProjection(mat proj);
	static void UpdateConstBuff(GraphicsBase&);

};