#pragma once
#include "pch.h"
#include "Utils.h"


namespace Renderer {
struct D3D11Renderbuffer : Renderbuffer {
	wrl::ComPtr<ID3D11Texture2D> handle = nullptr;
	int32_t multiSampleCount = 0;

#pragma warning(push)
#pragma warning(disable : 26495)
	D3D11Renderbuffer(uint8_t type) : type(type) {};
#pragma warning(push)

#define RENDERBUFFER_COLOR 0
#define RENDERBUFFER_DEPTH 1
	uint8_t type = 0;
	union {
		struct {
			//FNA3D_SurfaceFormat format;
			wrl::ComPtr<ID3D11RenderTargetView> rtView = nullptr;
		} color;
		struct {
			DepthFormat format = DepthFormat::DEPTHFORMAT_D16;
			wrl::ComPtr<ID3D11DepthStencilView> dsView = nullptr;
		} depth;
	};



	~D3D11Renderbuffer() {
		if (type == RENDERBUFFER_COLOR)
			color.rtView->Release();
		if (type == RENDERBUFFER_DEPTH)
			depth.dsView->Release();
	};
};

};
