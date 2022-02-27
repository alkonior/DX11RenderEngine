#pragma once
#include "pch.h"
#include "Renderer.h"
#include "TextureData.h"

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
		Renderer::Texture* texture;
		size_t width;
		size_t height;

	};

public:
	TexturesManager(Renderer::IRenderer* renderer);

	void RegTexture(void* tx, int width, int height, bool mipmap, size_t id);
	void RegTexture(const TextureData& tx, size_t id);
	void UpdateTexture(const TextureData& tx, size_t id);
	void UpdateTexture(const ImageUpdate& updateData);

	
	
	void ReleaseTexture(size_t id);
	TextureCache GetImg(size_t id);
	//void RegImgFromFile(GraphicsBase gfx, size_t width, size_t height, LPCSTR file, LPCSTR name);

	~TexturesManager();
private:
	std::map<size_t, TextureCache> textures;

};

