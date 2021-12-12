#pragma once
#include "pch.h"
#include "GraphicsException.h"
#include "GraphicsBase.h"

#ifdef _DEBUG
#include "DxgiInfoManager.h"
#endif

struct DefaultPixelShader {

	static void Init(GraphicsBase&, LPCWSTR);

	static void Bind(GraphicsBase&);

	static void Release();

	static wrl::ComPtr<ID3D11PixelShader> pPixelShader;
	static BYTE* data;
	static size_t dataSize;

};