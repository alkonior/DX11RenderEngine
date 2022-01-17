#pragma once
#include "pch.h"
#include "GraphicsBase.h"
#include "Texture.h"


class TexturesManager {
public:

	struct TextureCache {
		Renderer::Texture* texture;
		size_t width;
		size_t height;

	};

public:
	TexturesManager();

	void RegTexture(GraphicsBase& gfx, const TextureData& tx, size_t id);
	void ReeaseTexture(size_t id);
	TextureCache GetImg(GraphicsBase& gfx, size_t id);
	//void RegImgFromFile(GraphicsBase gfx, size_t width, size_t height, LPCSTR file, LPCSTR name);

private:
	std::map<size_t, TextureCache> textures;

};

