#pragma once
#include "pch.h"
#include "Utils.h"
namespace Renderer {

struct D3D11Texture : public Texture{
	D3D11Texture(wrl::ComPtr<ID3D11Texture2D> handle,wrl::ComPtr<ID3D11ShaderResourceView> shaderView,int32_t levelCount,uint8_t isRenderTarget,size_t width,size_t height,wrl::ComPtr <ID3D11RenderTargetView> rtView) :
		handle(handle), shaderView(shaderView),levelCount(levelCount), isRenderTarget(isRenderTarget), width(width), height(height), rtView(rtView), staging(nullptr){};
	D3D11Texture() = default;

	/* D3D Handles */
	wrl::ComPtr<ID3D11Texture2D> handle; /* ID3D11Texture2D or ID3D11Texture3D */
	wrl::ComPtr<ID3D11ShaderResourceView> shaderView;

	/* Basic Info */
	int32_t levelCount;
	uint8_t isRenderTarget;


	size_t width;
	size_t height;
	wrl::ComPtr <ID3D11RenderTargetView> rtView;


	wrl::ComPtr<ID3D11Texture2D> staging; /* ID3D11Texture2D or ID3D11Texture3D */
	static D3D11Texture NullTexture;
};

};