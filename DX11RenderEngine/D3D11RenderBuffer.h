#pragma once
#include "pch.h"
#include "Utils.h"


namespace Renderer {
struct D3D11Renderbuffer : Renderbuffer {
	wrl::ComPtr<ID3D11Texture2D> handle;
	int32_t multiSampleCount;
	
	D3D11Renderbuffer(uint8_t type) : type(type) {};
	

#define RENDERBUFFER_COLOR 0
#define RENDERBUFFER_DEPTH 1
	uint8_t type;
	union {
		struct {
			//FNA3D_SurfaceFormat format;
			wrl::ComPtr<ID3D11RenderTargetView> rtView;
		} color;
		struct {
			DepthFormat format;
			wrl::ComPtr<ID3D11DepthStencilView> dsView;
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