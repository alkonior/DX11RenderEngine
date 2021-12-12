#include "pch.h"
#include <filesystem>
#include "DefaultVertexShader.h"


wrl::ComPtr<ID3D11VertexShader> DefaultVertexShader::pVertexShader = nullptr;
wrl::ComPtr<ID3D11InputLayout> DefaultVertexShader::pInputLayout = nullptr;
BYTE* DefaultVertexShader::data = nullptr;
size_t DefaultVertexShader::dataSize = 0;
VertexShaderCosntBuffer DefaultVertexShader::localBuffer;
wrl::ComPtr<ID3D11Buffer> DefaultVertexShader::pConstantBuffer;

const D3D11_INPUT_ELEMENT_DESC DefaultVertexShader::inputLayout[ ] =
{
		{"Position",0,DXGI_FORMAT_R32G32_FLOAT,  0,                           0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"Color",   0,DXGI_FORMAT_R8G8B8A8_UNORM,0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
};

wrl::ComPtr<ID3D10Blob> VS = nullptr;
wrl::ComPtr<ID3DBlob> vsErrorBlob = nullptr;

void DefaultVertexShader::Init(GraphicsBase& gfx, LPCWSTR dirr) {

	HRESULT hr;
	{
		auto test = std::wstring(dirr) + L"\\VertexShader.hlsl";
		auto cwd = std::filesystem::current_path();
		HRESULT hrt = D3DCompileFromFile(test.c_str(), NULL, NULL, "main", "vs_4_0", NULL, NULL, &VS, &vsErrorBlob);
		if (FAILED(hrt)) {
			if (vsErrorBlob) {
				OutputDebugStringA((char*)vsErrorBlob->GetBufferPointer());
			}
			GFX_THROW_INFO(hrt);
		}
		data = (BYTE*)VS->GetBufferPointer();
		dataSize = VS->GetBufferSize();
	}



	GFX_THROW_INFO(gfx.pDevice->CreateVertexShader(data, dataSize, nullptr, &pVertexShader));
	GFX_THROW_INFO(gfx.pDevice->CreateInputLayout(inputLayout, (UINT)std::size(inputLayout), data, dataSize, &pInputLayout));



	D3D11_BUFFER_DESC cbd;
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.Usage = D3D11_USAGE_DEFAULT;
	cbd.CPUAccessFlags = 0u;
	cbd.MiscFlags = 0u;
	cbd.ByteWidth = sizeof(localBuffer);
	cbd.StructureByteStride = 0u;
	D3D11_SUBRESOURCE_DATA csd = {};
	csd.pSysMem = &localBuffer;
	GFX_THROW_INFO(gfx.pDevice->CreateBuffer(&cbd, &csd, &pConstantBuffer));

}

void DefaultVertexShader::Bind(GraphicsBase& gfx) {

	gfx.pContext->VSSetShader(pVertexShader.Get(), nullptr, 0u);
	gfx.pContext->IASetInputLayout(pInputLayout.Get());
	gfx.pContext->VSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf());

}

void DefaultVertexShader::BindViewport(GraphicsBase& gfx, D3D11_VIEWPORT vp) {
	gfx.pContext->RSSetViewports(1u, &vp);
}

void DefaultVertexShader::Release() {
	pConstantBuffer->Release();
	pInputLayout->Release();
	pVertexShader->Release();
}

void DefaultVertexShader::SetWorld(mat world) {
	localBuffer.world = world.Transpose();
}

void DefaultVertexShader::SetView(mat view) {
	localBuffer.view = view.Transpose();
}

void DefaultVertexShader::SetProjection(mat proj) {
	localBuffer.projection = proj.Transpose();
}

void DefaultVertexShader::UpdateConstBuff(GraphicsBase& gfx) {
	gfx.pContext->UpdateSubresource(
		pConstantBuffer.Get(), 0, 0,
		&localBuffer, 0, 0);

}

