#pragma once
#include "pch.h"
#include "IRenderer/Utils.h"
#include "../GraphicsVirtualMachine/BaseStructures.h"

namespace Renderer {

struct D3D11Texture : public Texture {
	D3D11Texture(wrl::ComPtr<ID3D11Texture2D> handle, wrl::ComPtr<ID3D11ShaderResourceView> shaderView, int32_t levelCount, uint8_t isRenderTarget, size_t width, size_t height, wrl::ComPtr <ID3D11RenderTargetView> rtView) :
		handle(handle), shaderView(shaderView), levelCount(levelCount), isRenderTarget(isRenderTarget), width(width), height(height), rtView(rtView), staging(nullptr) {};
	D3D11Texture() = default;

	/* D3D Handles */
	wrl::ComPtr<ID3D11Texture2D> handle; /* ID3D11Texture2D or ID3D11Texture3D */
	GVM::Resource* resource;
	wrl::ComPtr<ID3D11ShaderResourceView> shaderView = nullptr;
	GVM::ShaderResourceView* shView;
	wrl::ComPtr<ID3D11UnorderedAccessView> uaView = nullptr;
	GVM::UATargetView* uaTestView;

	/* Basic Info */
	int32_t levelCount = 0;
	uint8_t isRenderTarget = false;
	SurfaceFormat format;


	size_t width = 0;
	size_t height = 0;
	size_t cubeSize = 0;
	wrl::ComPtr <ID3D11RenderTargetView> rtView;
	GVM::RenderTargetView* rtTestView;


	wrl::ComPtr<ID3D11Texture2D> staging; /* ID3D11Texture2D or ID3D11Texture3D */
	static D3D11Texture NullTexture;
};

};