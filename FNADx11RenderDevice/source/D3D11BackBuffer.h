#pragma once

#include "winHandler.h"
#include "IRenderer.h"

namespace Renderer {
struct D3D11Backbuffer {
#define BACKBUFFER_TYPE_NULL 0
#define BACKBUFFER_TYPE_D3D11 1
	uint8_t type;

	int32_t width;
	int32_t height;
	DepthFormat depthFormat;
	int32_t multiSampleCount;
	wrl::ComPtr<ID3D11Texture2D> depthStencilBuffer;
	wrl::ComPtr<ID3D11DepthStencilView> depthStencilView;
	wrl::ComPtr<ID3D11Texture2D> stagingBuffer;
	struct {
		/* Color */
		//SurfaceFormat surfaceFormat;
		wrl::ComPtr<ID3D11Texture2D> colorBuffer;
		wrl::ComPtr<ID3D11RenderTargetView> colorView;
		wrl::ComPtr<ID3D11ShaderResourceView> shaderView;

		/* Multisample */
		wrl::ComPtr<ID3D11Texture2D> resolveBuffer;
	} d3d11;
};

};