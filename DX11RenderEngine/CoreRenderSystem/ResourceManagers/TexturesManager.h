#pragma once
#include "pch.h"
#include "IRenderer/IRenderer.h"
#include "../Utils/TextureData.h"

struct ImageBox {
	int32_t x; int32_t y;
	int32_t w; int32_t h;
};

struct ImageUpdate {
	size_t id;
	ImageBox box;
	int32_t width; int32_t height;
	int32_t level;
	uint8_t* data;
};

class TexturesManager {
	Renderer::IRenderer* renderer;
public:

	struct TextureCache {
		Renderer::Texture* texture = nullptr;
		size_t width;
		size_t height;

	};

public:
	TexturesManager();

	void RegTexture(void* tx, int width, int height, bool mipmap, size_t id);
	void RegTexture(const TextureData& tx, size_t id);
	void UpdateTexture(const TextureData& tx, size_t id);
	void UpdateTexture(const ImageUpdate& updateData);

	
	
	void ReleaseTexture(size_t id);
	TextureCache GetImg(size_t id);
	//void RegImgFromFile(GraphicsBase gfx, size_t width, size_t height, LPCSTR file, LPCSTR name);

	//RenderTeargets::

	Renderer::Renderbuffer* depthRBuffer;
	

	Renderer::Texture* diffuseColor;
	Renderer::Texture* directLights;
	Renderer::Texture* luminance;
	Renderer::Texture* alphaSurfaces;

	Renderer::RenderTargetBinding diffuseColorRT;
	Renderer::RenderTargetBinding directLightsRT;
	Renderer::RenderTargetBinding luminanceRT;
	Renderer::RenderTargetBinding alphaSurfacesRT;
	
	Renderer::Texture* bloomBlured;
	Renderer::RenderTargetBinding bloomBluredRT;
	
	Renderer::Texture* preFXAAcolor;
	Renderer::RenderTargetBinding preFXAAcolorRT;
	
	void CreateRenderTarget(Renderer::SurfaceFormat format, size_t width, size_t height, Renderer::Texture*& texture, Renderer::RenderTargetBinding& renderTarget);

	
	//Renderer::Texture* blumeMask;

	std::unordered_map<Renderer::Texture*, Renderer::Texture*> renderTargets;
		
	~TexturesManager();
private:
	std::map<size_t, TextureCache> textures;

	
};

