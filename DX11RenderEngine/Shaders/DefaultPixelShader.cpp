#include "pch.h"
#include <filesystem>
#include "DefaultPixelShader.h"


wrl::ComPtr<ID3D11PixelShader> DefaultPixelShader::pPixelShader = nullptr;
BYTE* DefaultPixelShader::data = nullptr;
size_t DefaultPixelShader::dataSize = 0;

wrl::ComPtr<ID3D10Blob> PS = nullptr;
wrl::ComPtr<ID3DBlob> psErrorBlob = nullptr;

void DefaultPixelShader::Init(GraphicsBase& gfx, LPCWSTR dirr) {
	HRESULT hr;
	{
		auto test = std::wstring(dirr) + L"\\PixelShader.hlsl";
		auto cwd = std::filesystem::current_path();
		HRESULT hrt = D3DCompileFromFile(test.c_str(), NULL, NULL, "main", "ps_4_0", NULL, NULL, &PS, &psErrorBlob);
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

void DefaultPixelShader::Bind(GraphicsBase& gfx) {
	gfx.pContext->PSSetShader(pPixelShader.Get(), nullptr, 0u);
}

void DefaultPixelShader::Release() {
	pPixelShader->Release();
}
