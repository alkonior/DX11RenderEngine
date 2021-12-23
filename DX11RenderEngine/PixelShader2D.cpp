//#include "pch.h"
//#include <filesystem>
//#include "PixelShader2D.h"
//
//
//wrl::ComPtr<ID3D11PixelShader> PixelShader2D::pPixelShader;
//
//wrl::ComPtr<ID3D10Blob> PS = nullptr;
//wrl::ComPtr<ID3D10Blob> psErrorBlob = nullptr;
//
//void PixelShader2D::Init(GraphicsBase& gfx, LPCWSTR dirr) {
//	wrl::ComPtr<ID3DBlob> buff;
//	D3DReadFileToBlob((std::wstring(dirr) + L"\\PixelShader2D.hlsl").c_str(), &buff);
//	Init(gfx, buff->GetBufferPointer(), buff->GetBufferSize());
//}
//
//void PixelShader2D::Init(GraphicsBase& gfx, LPVOID data, size_t size) {
//	HRESULT hr;
//	{
//		HRESULT hrt = D3DCompile(data, size, NULL, NULL, NULL, "main", "ps_4_0", NULL, NULL, &PS, &psErrorBlob);
//		if (FAILED(hrt)) {
//			if (psErrorBlob) {
//				OutputDebugStringA((char*)psErrorBlob->GetBufferPointer());
//			}
//			GFX_THROW_INFO(hrt);
//		}
//	}
//
//	GFX_THROW_INFO(gfx.pDevice->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), nullptr, &pPixelShader));
//}
//
//void PixelShader2D::Bind(GraphicsBase& gfx) {
//	gfx.pContext->PSSetShader(pPixelShader.Get(), nullptr, 0u);
//}
//
//void PixelShader2D::Release() {
//	//pPixelShader->Release();
//}
