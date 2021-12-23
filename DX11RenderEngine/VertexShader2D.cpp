//#include "pch.h"
//#include <filesystem>
//#include "VertexShader2D.h"
//
//
//wrl::ComPtr<ID3D11VertexShader> VertexShader2D::pVertexShader = nullptr;
//wrl::ComPtr<ID3D11InputLayout> VertexShader2D::pInputLayout = nullptr;
//VertexShaderCosntBuffer VertexShader2D::localBuffer;
//wrl::ComPtr<ID3D11Buffer> VertexShader2D::pConstantBuffer = nullptr;
//
//const D3D11_INPUT_ELEMENT_DESC VertexShader2D::inputLayout[ ] =
//{
//		{"Position",  0, DXGI_FORMAT_R32G32_FLOAT,  0,                           0, D3D11_INPUT_PER_VERTEX_DATA, 0},
//		{"TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,  0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
//};
//
//wrl::ComPtr<ID3D10Blob> VS = nullptr;
//wrl::ComPtr<ID3DBlob> vsErrorBlob = nullptr;
//
//void VertexShader2D::Init(GraphicsBase& gfx, LPCWSTR dirr) {
//
//	wrl::ComPtr<ID3DBlob> buff;
//	D3DReadFileToBlob((std::wstring(dirr) + L"\\VertexShader2D.hlsl").c_str(), &buff);
//	Init(gfx, buff->GetBufferPointer(), buff->GetBufferSize());
//
//}
//
//void VertexShader2D::Init(GraphicsBase& gfx, LPVOID data, size_t size) {
//
//	HRESULT hr;
//	{
//		HRESULT hrt = D3DCompile(data, size, NULL, NULL, NULL, "main", "vs_4_0", NULL, NULL, &VS, &vsErrorBlob);
//		if (FAILED(hrt)) {
//			if (vsErrorBlob) {
//				OutputDebugStringA((char*)vsErrorBlob->GetBufferPointer());
//			}
//			GFX_THROW_INFO(hrt);
//		}
//	}
//
//	GFX_THROW_INFO(gfx.pDevice->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), nullptr, &pVertexShader));
//	GFX_THROW_INFO(gfx.pDevice->CreateInputLayout(inputLayout, (UINT)std::size(inputLayout), VS->GetBufferPointer(), VS->GetBufferSize(), &pInputLayout));
//
//
//
//	D3D11_BUFFER_DESC cbd;
//	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
//	cbd.Usage = D3D11_USAGE_DEFAULT;
//	cbd.CPUAccessFlags = 0u;
//	cbd.MiscFlags = 0u;
//	cbd.ByteWidth = sizeof(localBuffer);
//	cbd.StructureByteStride = 0u;
//	D3D11_SUBRESOURCE_DATA csd = {};
//	csd.pSysMem = &localBuffer;
//	GFX_THROW_INFO(gfx.pDevice->CreateBuffer(&cbd, &csd, &pConstantBuffer));
//}
//
//
//void VertexShader2D::Bind(GraphicsBase& gfx) {
//
//	gfx.pContext->VSSetShader(pVertexShader.Get(), nullptr, 0u);
//	gfx.pContext->IASetInputLayout(pInputLayout.Get());
//	gfx.pContext->VSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf());
//
//}
//
//void VertexShader2D::BindViewport(GraphicsBase& gfx, D3D11_VIEWPORT vp) {
//	gfx.pContext->RSSetViewports(1u, &vp);
//}
//
//void VertexShader2D::Release() {
//	//pConstantBuffer->Release();
//	//pInputLayout->Release();
//	//pVertexShader->Release();
//}
//
//void VertexShader2D::SetTransform(mat transform, vec2 uvShift, vec2 uvScale) {
//	localBuffer.transform = transform.Transpose();
//	localBuffer.uvShift = uvShift;
//	localBuffer.uvScale = uvScale;
//}
//
//
//
//void VertexShader2D::UpdateConstBuff(GraphicsBase& gfx) {
//	gfx.pContext->UpdateSubresource(
//		pConstantBuffer.Get(), 0, 0,
//		&localBuffer, 0, 0);
//
//}
//
