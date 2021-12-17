#pragma once
#include "pch.h"
#include "GraphicsException.h"
#include "GraphicsBase.h"

#ifdef _DEBUG
#include "DxgiInfoManager.h"
#endif

struct PixelShader2D {

	static void Init(GraphicsBase&, LPCWSTR);
	static void Init(GraphicsBase&, LPVOID data, size_t size);

	static void Bind(GraphicsBase&);

	static void Release();

	static wrl::ComPtr<ID3D11PixelShader> pPixelShader;

};