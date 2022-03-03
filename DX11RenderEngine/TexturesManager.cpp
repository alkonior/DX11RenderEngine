#include "TexturesManager.h"

using namespace Renderer;
TexturesManager::TexturesManager(Renderer::IRenderer* renderer):renderer(renderer) {
	int width, height;
	renderer->GetBackbufferSize(&width, &height);

	diffuseColor = renderer->CreateTexture2D(SurfaceFormat::SURFACEFORMAT_COLOR, width, height, 1, true);
	directLights = renderer->CreateTexture2D(SurfaceFormat::SURFACEFORMAT_COLOR, width, height, 1, true);
	blumeMask    = renderer->CreateTexture2D(SurfaceFormat::SURFACEFORMAT_SINGLE, width, height, 1, true);
	blumeMask2    = renderer->CreateTexture2D(SurfaceFormat::SURFACEFORMAT_SINGLE, width, height, 1, true);
	alphaSurfaces = renderer->CreateTexture2D(SurfaceFormat::SURFACEFORMAT_COLOR, width, height, 1, true);

	renderTargets.push_back(diffuseColor);
	renderTargets.push_back(directLights);
	renderTargets.push_back(blumeMask);
	renderTargets.push_back(blumeMask2);
	renderTargets.push_back(alphaSurfaces);

	Viewport defVP = {
		0,0,width,height,0.0,1.0
	};
	
	diffuseColorRT.viewport = defVP;
	directLightsRT.viewport = defVP;
	blumeMaskRT.viewport = defVP;
	blumeMask2RT.viewport = defVP;
	alphaSurfacesRT.viewport = defVP;

	diffuseColorRT.texture = diffuseColor;
	directLightsRT.texture = directLights;
	blumeMaskRT.texture = blumeMask;
	blumeMask2RT.texture = blumeMask2;
	alphaSurfacesRT.texture = alphaSurfaces;

	diffuseColorRT.levelCount = 1;
	directLightsRT.levelCount = 1;
	blumeMaskRT.levelCount = 1;
	blumeMask2RT.levelCount = 1;
	alphaSurfacesRT.levelCount = 1;

	diffuseColorRT.multiSampleCount = 0;
	directLightsRT.multiSampleCount = 0;
	blumeMaskRT.multiSampleCount = 0;
	blumeMask2RT.multiSampleCount = 0;
	alphaSurfacesRT.multiSampleCount = 0;

}

void TexturesManager::RegTexture(void* data, int width, int height, bool mipmap, size_t id) {
	auto& pTexture = textures[id];
	if (pTexture.texture != nullptr) {
		ReleaseTexture(id);
	}
	//GFX_THROW_INFO(gfx.pDevice->CreateTexture2D(&textureDesc, &sd, &pTexture.texture));
	pTexture.texture = renderer->CreateTexture2D(SurfaceFormat::SURFACEFORMAT_COLOR, width, height, 1, 0);
	renderer->SetTextureData2D(pTexture.texture, 0, 0, width, height, 0, data, width * height * 4);
	pTexture.width = width;
	pTexture.height = height;
}

void TexturesManager::RegTexture(const TextureData& tx, size_t id) {
	auto&  pTexture = textures[id];
	if (pTexture.texture != nullptr) {
		ReleaseTexture(id);
	}
	//GFX_THROW_INFO(gfx.pDevice->CreateTexture2D(&textureDesc, &sd, &pTexture.texture));
	pTexture.texture = renderer->CreateTexture2D(SurfaceFormat::SURFACEFORMAT_COLOR, tx.GetWidth(), tx.GetHeight(), 1, 0);
	renderer->SetTextureData2D(pTexture.texture, 0, 0, tx.GetWidth(), tx.GetHeight(), 0, (void*)tx.GetBufferPtr(), tx.GetWidth()*tx.GetHeight() * 4);
	pTexture.width = tx.GetWidth();
	pTexture.height = tx.GetHeight();
}

void TexturesManager::UpdateTexture(const TextureData& tx, size_t id) {
	auto& pTexture = textures[id];
	if (pTexture.texture == nullptr) {
		RegTexture(tx, id);
	}
	else {
		renderer->SetTextureData2D(pTexture.texture, 0, 0, tx.GetWidth(), tx.GetHeight(), 0, (void*)tx.GetBufferPtr(), tx.GetWidth() * tx.GetHeight() * 4);
	}
}

void TexturesManager::UpdateTexture(const ImageUpdate& updateData) {
	auto& pTexture = textures[updateData.id];
	if (pTexture.texture == nullptr) {
		pTexture.texture = renderer->CreateTexture2D(SurfaceFormat::SURFACEFORMAT_COLOR, updateData.width, updateData.height, 1, 0);
		//return;
	}
	renderer->SetTextureData2D(pTexture.texture, updateData.box.x, updateData.box.y, updateData.box.w, updateData.box.h,
		updateData.level, updateData.data, 0);
}


void TexturesManager::ReleaseTexture(size_t id) {
	auto& pTexture = textures[id];
	if (pTexture.texture != nullptr) renderer->AddDisposeTexture(pTexture.texture);
	pTexture.texture = nullptr;
	//textures.erase(id);
}

TexturesManager::~TexturesManager() {
	for (auto& [key, texture]: textures) {
		ReleaseTexture(key);
	}
	
	for (auto& texture: renderTargets) {
		renderer->AddDisposeTexture(texture);
	}


}

TexturesManager::TextureCache TexturesManager::GetImg(size_t id) {
	return textures[id];	
}



