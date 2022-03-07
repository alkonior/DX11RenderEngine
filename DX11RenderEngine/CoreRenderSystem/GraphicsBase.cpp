#include "GraphicsBase.h"

using namespace Renderer;

PresentationParameters GraphicsBase::GenParams(HWND hWnd, size_t width, size_t height) {
	PresentationParameters pp;

	pp.isFullScreen = false;
	pp.backBufferWidth = width;
	pp.backBufferHeight = height;
	pp.deviceWindowHandle = hWnd;
	pp.multiSampleCount = 0;
	pp.depthStencilFormat = DepthFormat::DEPTHFORMAT_D32;
	pp.presentationInterval = PresentInterval::PRESENTINTERVAL_IMMEDIATE;

	return pp;
}

GraphicsBase::GraphicsBase(HWND hWnd, size_t width, size_t height)
	:width(width), height(height), renderer(GenParams(hWnd, width, height), 1),
	modelsManadger(&renderer), texturesManger(&renderer) {


		cameraProjection = matrix::CreatePerspective(width, height, 100, 1000);
}

void GraphicsBase::OnResize(size_t width, size_t height) {
	/*
	this->width = width;
	this->height = height;
	// Release existing DirectX views and buffers
	pDSV->Release();
	pTarget->Release();

	// Resize the underlying swap chain buffers
	pSwap->ResizeBuffers(
		1,
		width,
		height,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		0);

	// Recreate the render target view for the back buffer
	// texture, then release our local texture reference
	wrl::ComPtr<ID3D11Texture2D> pBackBuffer;
	pSwap->GetBuffer(0, __uuidof(ID3D11Texture2D), &pBackBuffer);
	pDevice->CreateRenderTargetView(pBackBuffer.Get(), 0, &pTarget);

	// Set up the description of the texture to use for the depth buffer
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width = width;
	depthStencilDesc.Height = height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;

	// Create the depth buffer and its view, then
	// release our reference to the texture
	wrl::ComPtr<ID3D11Texture2D> pDepthBuffer;
	pDevice->CreateTexture2D(&depthStencilDesc, 0, &pDepthBuffer);
	pDevice->CreateDepthStencilView(pDepthBuffer.Get(), 0, &pDSV);

	// Bind the views to the pipeline, so rendering properly
	// uses their underlying textures
	pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), pDSV.Get());

	// Lastly, set up a viewport so we render into
	// to correct portion of the window
	D3D11_VIEWPORT vp;
	vp.Width = width * 1.0f;
	vp.Height = height * 1.0f;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	pContext->RSSetViewports(1u, &vp);
	*/
}

void GraphicsBase::SetCameraPosition(float3 position) {
	cameraPosition = position;
}

void GraphicsBase::SetCameraMatrix(matrix position) {
	viewMatrix = position;
}

void GraphicsBase::SetCameraProjection(matrix projection) {
	cameraProjection = projection;
}
