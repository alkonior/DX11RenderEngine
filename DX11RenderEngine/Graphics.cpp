#include "pch.h"
#include "Graphics.h"
#include <sstream>
#include <cmath>
#include <d3dcompiler.h>
#include <filesystem>
#include <resource.h>

#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"D3DCompiler.lib")


Graphics::Graphics(HWND hWnd, size_t width, size_t height)
	:GraphicsBase(hWnd, width, height), manager2D(*this) {}

void Graphics::EndFrame() {
	HRESULT hr;
	auto& gfx = *this;

	manager2D.Present(gfx);



#ifndef NDEBUG
	infoManager.Set();
#endif
	if (FAILED(hr = pSwap->Present(1u, 0u))) {
		if (hr == DXGI_ERROR_DEVICE_REMOVED) {
			throw GFX_DEVICE_REMOVED_EXCEPT(pDevice->GetDeviceRemovedReason());
		} else {
			throw GFX_EXCEPT(hr);
		}
	}
}

void Graphics::ClearBuffer(sm::Vector4 color) noexcept {

	pContext->ClearRenderTargetView(pTarget.Get(), reinterpret_cast<float*>(&color));
	pContext->ClearDepthStencilView(pDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
}



void Graphics::DrawImg(size_t id, size_t x, size_t y, size_t width, size_t height) {
	manager2D.Draw(texturesManger.GetImg(*this, id), x, y, width, height);
}


void Graphics::DrawImg(size_t id, size_t top, size_t left, size_t texW, size_t texH, size_t x, size_t y, size_t width, size_t height) {
	manager2D.Draw(texturesManger.GetImg(*this, id),top, left, texW, texH, x, y, width, height);
}

void Graphics::RegisterImg(size_t id, const Texture& text) {
	texturesManger.RegTexture(*this, text, id);
}



/*
	HRESULT hr;

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

	// create vertex buffer (1 2d triangle at center of screen)
	Vertex vertices[ ] =
	{
		{  0.0f,   0.5f,    255,0,0,0 },
		{  0.5f,  -0.5f,    0,255,0,0 },
		{ -0.5f,  -0.5f,    0,0,255,0 },
		{ -0.3f,   0.3f,    0,255,0,0 },
		{  0.3f,   0.3f,    0,0,255,0 },
		{  0.0f,  -0.8f,    255,0,0,0 },
	};
	vertices[0].color.g = 255;
	wrl::ComPtr<ID3D11Buffer> pVertexBuffer;
	D3D11_BUFFER_DESC bd = {};
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0u;
	bd.MiscFlags = 0u;
	bd.ByteWidth = sizeof(vertices);
	bd.StructureByteStride = sizeof(Vertex);
	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = vertices;
	GFX_THROW_INFO(pDevice->CreateBuffer(&bd, &sd, &pVertexBuffer));

	// Bind vertex buffer to pipeline
	const UINT stride = sizeof(Vertex);
	const UINT offset = 0u;
	pContext->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);


	// create index buffer
	const unsigned short indices[ ] =
	{
		0,1,2,
		0,2,3,
		0,4,1,
		2,1,5,
	};
	wrl::ComPtr<ID3D11Buffer> pIndexBuffer;
	D3D11_BUFFER_DESC ibd = {};
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.CPUAccessFlags = 0u;
	ibd.MiscFlags = 0u;
	ibd.ByteWidth = sizeof(indices);
	ibd.StructureByteStride = sizeof(unsigned short);
	D3D11_SUBRESOURCE_DATA isd = {};
	isd.pSysMem = indices;
	GFX_THROW_INFO(pDevice->CreateBuffer(&ibd, &isd, &pIndexBuffer));

	// bind index buffer
	pContext->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);


	// create pixel shader
	DefaultPixelShader::Bind(*this);

	DefaultVertexShader::Bind(*this);


	// bind render target
	pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), nullptr);


	// Set primitive topology to triangle list (groups of 3 vertices)
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	// configure viewport
	D3D11_VIEWPORT vp;
	vp.Width = 800;
	vp.Height = 600;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	pContext->RSSetViewports(1u, &vp);


	GFX_THROW_INFO_ONLY(pContext->DrawIndexed((UINT)std::size(indices), 0u, 0u));*/