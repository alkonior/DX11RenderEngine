#include "pch.h"
#include <filesystem>
#include "PixelShader2D.h"


wrl::ComPtr<ID3D11PixelShader> PixelShader2D::pPixelShader = nullptr;
BYTE* PixelShader2D::data = nullptr;
size_t PixelShader2D::dataSize = 0;

wrl::ComPtr<ID3D10Blob> PS = nullptr;
wrl::ComPtr<ID3D10Blob> psErrorBlob = nullptr;

void PixelShader2D::Init(GraphicsBase& gfx, LPCWSTR dirr) {
	wrl::ComPtr<ID3DBlob> buff;
	D3DReadFileToBlob((std::wstring(dirr) + L"\\PixelShader2D.hlsl").c_str(), &buff);
	Init(gfx, buff->GetBufferPointer(), buff->GetBufferSize());
}

void PixelShader2D::Init(GraphicsBase& gfx, LPVOID data, size_t size) {
	HRESULT hr;
	{
		HRESULT hrt = D3DCompile(data, size, NULL, NULL, NULL, "main", "ps_4_0", NULL, NULL, &PS, &psErrorBlob);
		if (FAILED(hrt)) {
			if (psErrorBlob) {
				OutputDebugStringA((char*)psErrorBlob->GetBufferPointer());
			}
			GFX_THROW_INFO(hrt);
		}
		data = (BYTE*)PS->GetBufferPointer();
		dataSize = PS->GetBufferSize();
	}

	GFX_THROW_INFO(gfx.pDevice->CreatePixelShader(data, dataSize, nullptr, &pPixelShader));
}

void PixelShader2D::Bind(GraphicsBase& gfx) {
	gfx.pContext->PSSetShader(pPixelShader.Get(), nullptr, 0u);
}

void PixelShader2D::Release() {
	pPixelShader->Release();
}
