#pragma once
//#include "winHandler.h"
#include "IRenderer/Utils.h"
#include "GVMBaseStructures/IRenderClasses.h"


namespace Renderer {

struct D3D11Texture : public Texture {
	D3D11Texture(int32_t levelCount, uint8_t isRenderTarget, size_t width, size_t height) :
		levelCount(levelCount), isRenderTarget(isRenderTarget), width(width), height(height) {};
	D3D11Texture() = default;

	/* D3D Handles */
	GVM::Resource* resource;
	GVM::ShaderResourceView* shView;
	GVM::UATargetView* uaTestView;

	/* Basic Info */
	int32_t levelCount = 0;
	uint8_t isRenderTarget = false;
	SurfaceFormat format;


	size_t width = 0;
	size_t height = 0;
	GVM::RenderTargetView* rtTestView;


	static D3D11Texture NullTexture;
};

};