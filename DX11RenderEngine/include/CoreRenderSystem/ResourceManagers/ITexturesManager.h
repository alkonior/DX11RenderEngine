#pragma once

#include "IRenderer.h"
#include "TextureData.h"
//#include "DrawData.h"

#include <map>

#include "SID.h"



class ITexturesManager {
public:
	Renderer::IRenderer* renderDevice = nullptr;

	struct TextureCache {
		Renderer::Texture* texture = nullptr;
		size_t width = 0;
		size_t height = 0;
	};

public:
	ITexturesManager(Renderer::IRenderer* renderDevice): renderDevice(renderDevice){};

	virtual void RegTexture(void* tx, int width, int height, bool mipmap, size_t id) = 0;
	virtual void RegTexture(const TextureData& tx, size_t id) = 0;
	
	virtual void UpdateTexture(const TextureData& tx, size_t id) = 0;
	virtual void UpdateTexture(const ImageUpdate& updateData) = 0;
	
	
	virtual void ReleaseTexture(size_t id) = 0;
	virtual TextureCache GetImg(size_t id) = 0;


	struct RenderTargetDescription{
		Renderer::SurfaceFormat format = Renderer::SURFACEFORMAT_COLOR;
		bool isUA = false;
		size_t width = 0;
		size_t height = 0;
	};

	virtual const Renderer::RenderTargetBinding& CreatePublicRenderTarget(
		Renderer::string_id id,
		const RenderTargetDescription& description) = 0;

	virtual const Renderer::RenderTargetBinding& CreatePrivateRenderTarget(
		Renderer::string_id id,
		const RenderTargetDescription& description,
		Renderer::Texture*& diffuseColor,
		Renderer::RenderTargetBinding& diffuseColorRT
	) = 0;

	virtual Renderer::RenderTargetBinding* GetRenderTarget(Renderer::string_id id) = 0;
	
	Renderer::Renderbuffer* depthBuffer = nullptr;
	
	
	virtual ~ITexturesManager() = default;

	
};

