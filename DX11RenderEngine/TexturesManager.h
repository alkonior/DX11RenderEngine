#pragma once
#include "pch.h"
#include "GraphicsBase.h"
#include "TextureData.h"


class TexturesManager {
public:
	Renderer::IRenderer* renderer;

	struct TextureCache {
		Renderer::Texture* texture;
		size_t width;
		size_t height;

	};

public:
	TexturesManager(Renderer::IRenderer* renderer);

	void RegTexture(const TextureData& tx, size_t id);
	void UpdateTexture(const TextureData& tx, size_t id);
	void UpdateTexture(size_t id, int32_t x, int32_t y, int32_t w, int32_t h, int32_t level, void* data, int32_t dataLength);
	void ReleaseTexture(size_t id);
	TextureCache GetImg(size_t id);
	//void RegImgFromFile(GraphicsBase gfx, size_t width, size_t height, LPCSTR file, LPCSTR name);

	~TexturesManager();
private:
	std::map<size_t, TextureCache> textures;

};

