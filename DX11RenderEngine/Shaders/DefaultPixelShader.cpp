#include "pch.h"

#include "DefaultPixelShader.h"


#include "PixelShader.inl"


wrl::ComPtr<ID3D11PixelShader> DefaultPixelShader::pPixelShader = nullptr;
const BYTE* DefaultPixelShader::data = g_main;
const size_t DefaultPixelShader::dataSize = std::size(g_main);

void DefaultPixelShader::Init(GraphicsBase& gfx) {
	HRESULT hr;
	GFX_THROW_INFO(gfx.pDevice->CreatePixelShader(data, dataSize, nullptr, &pPixelShader));
}

void DefaultPixelShader::Bind(GraphicsBase& gfx) {
	gfx.pContext->PSSetShader(pPixelShader.Get(), nullptr, 0u);
}

void DefaultPixelShader::Release() {
	pPixelShader->Release();
}
