#pragma once

#include "IRenderer.h"
#include "Utils/TextureData.h"
#include "DrawData.h"

#include <map>

class TexturesManager {
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

	Renderer::Renderbuffer* depthBuffer;
	

	Renderer::Texture* diffuseColor;
	Renderer::RenderTargetBinding diffuseColorRT;
	
	Renderer::Texture* lightColor;
	Renderer::RenderTargetBinding lightColorRT;
	
	Renderer::Texture* alphaSurfaces;
	Renderer::RenderTargetBinding alphaSurfacesRT;
	
	Renderer::Texture* bloomMask;
	Renderer::RenderTargetBinding bloomMaskRT;
	
	Renderer::Texture* velocityField;
	Renderer::RenderTargetBinding velocityFieldRT;
	
	Renderer::Texture* blurMask;
	Renderer::RenderTargetBinding blurMaskRT;
	
	Renderer::Texture* preAAcolor;
	Renderer::RenderTargetBinding preAAcolorRT;
	
	Renderer::Texture* pastColor;
	Renderer::RenderTargetBinding pastColorRT;
	Renderer::Texture* pastDepth;
	Renderer::RenderTargetBinding pastDepthRT;

	
	Renderer::Texture* normalsField;
	Renderer::RenderTargetBinding normalsFieldRT;
	
	Renderer::Texture* oclusionField;
	Renderer::RenderTargetBinding oclusionFieldRT;
	
	
	void CreateRenderTarget(Renderer::SurfaceFormat format, size_t width, size_t height, Renderer::Texture*& texture, Renderer::RenderTargetBinding& renderTarget);

	void CreateUATarget(Renderer::SurfaceFormat format, size_t width, size_t height, Renderer::Texture*& texture, Renderer::RenderTargetBinding& renderTarget);

	
	//Renderer::Texture* blumeMask;

	std::unordered_map<Renderer::Texture**, Renderer::Texture*> renderTargets;
		
	~TexturesManager();
private:
	std::map<size_t, TextureCache> textures;

	
};

