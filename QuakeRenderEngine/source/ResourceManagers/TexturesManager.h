#pragma once

#include "Utils/TextureData.h"
#include "DrawData.h"

#include <map>
#include <unordered_map>

#include "CoreRenderSystem/ResourceManagers/ITexturesManager.h"

class TexturesManager : public ITexturesManager {
public:


public:
	TexturesManager();

	virtual void RegTexture(void* tx, int width, int height, bool mipmap, size_t id);
	virtual void RegTexture(const TextureData& tx, size_t id);
	virtual void UpdateTexture(const TextureData& tx, size_t id);
	virtual void UpdateTexture(const ImageUpdate& updateData);

	
	
	virtual void ReleaseTexture(size_t id);
	virtual TextureCache GetImg(size_t id);
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

		
	virtual ~TexturesManager();

	virtual const Renderer::RenderTargetBinding& CreatePublicRenderTarget(
			Renderer::string_id id,
			const RenderTargetDescription& description) override;

	virtual const Renderer::RenderTargetBinding& CreatePrivateRenderTarget(
		Renderer::string_id id,
		const RenderTargetDescription& description,
		Renderer::Texture*& diffuseColor,
		Renderer::RenderTargetBinding& diffuseColorRT
	) override;

	virtual const Renderer::RenderTargetBinding& GetRenderTarget(Renderer::string_id id) override;
	
private:
	std::map<Renderer::string_id, TextureCache> textures;
	
	std::unordered_map<Renderer::string_id, Renderer::RenderTargetBinding> publicRenderTargets;
	std::unordered_map<Renderer::string_id, Renderer::RenderTargetBinding> privateRenderTargets;

	
};

