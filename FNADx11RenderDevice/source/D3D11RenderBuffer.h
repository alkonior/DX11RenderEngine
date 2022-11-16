#pragma once
#include "IRenderer/Utils.h"
#include "GVMBaseStructures/IRenderClasses.h"


namespace Renderer {
struct D3D11Renderbuffer : Renderbuffer {
	GVM::Resource* resource = nullptr;
	int32_t multiSampleCount = 0;

#pragma warning(push)
#pragma warning(disable : 26495)
	D3D11Renderbuffer(uint8_t type) : type(type) {};
#pragma warning(push)

#define RENDERBUFFER_COLOR 0
#define RENDERBUFFER_DEPTH 1
	const uint8_t type = 0;
	union {
		struct {
			SurfaceFormat format;
			GVM::RenderTargetView* nRsView = nullptr;
		} color;
		struct {
			DepthFormat format = DepthFormat::DEPTHFORMAT_D16;
			GVM::DepthStencilView* nDsView = nullptr;
			GVM::ShaderResourceView* nShView = nullptr;
		} depth;
	};


	~D3D11Renderbuffer() {
		//if (type == RENDERBUFFER_COLOR)
		//	color.rtView->Release();
		//if (type == RENDERBUFFER_DEPTH)
		//	depth.dsView->Release();
	};
};

};
