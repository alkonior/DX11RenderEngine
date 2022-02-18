#include "TexturesManager.h"


TexturesManager::TexturesManager(Renderer::IRenderer* renderer):renderer(renderer) {}

void TexturesManager::RegTexture(void* data, int width, int height, bool mipmap, size_t id) {
	auto& pTexture = textures[id];
	if (pTexture.texture != nullptr) {
		ReleaseTexture(id);
	}
	//GFX_THROW_INFO(gfx.pDevice->CreateTexture2D(&textureDesc, &sd, &pTexture.texture));
	pTexture.texture = renderer->CreateTexture2D(width, height, 0, 0);
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
	pTexture.texture = renderer->CreateTexture2D(tx.GetWidth(), tx.GetHeight(), 0, 0);
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

void TexturesManager::UpdateTexture(size_t id, int32_t x, int32_t y, int32_t w, int32_t h, int32_t level, void* data) {
	auto& pTexture = textures[id];
	if (pTexture.texture == nullptr) {
		return;
	}
	renderer->SetTextureData2D(pTexture.texture, x, y, w, h, level, data, 0);
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
}

TexturesManager::TextureCache TexturesManager::GetImg(size_t id) {
	return textures[id];	
}



