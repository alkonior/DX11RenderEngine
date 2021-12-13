#pragma once
#include "pch.h"
#include "GraphicsBase.h"
#include "Texture.h"


class TexturesManager {
public:
	TexturesManager();

	void RegTexture(GraphicsBase gfx, Texture& tx, char* name);
	wrl::ComPtr<ID3D11ShaderResourceView> BindImg(GraphicsBase gfx, char* name);
	//void RegImgFromFile(GraphicsBase gfx, size_t width, size_t height, LPCSTR file, LPCSTR name);

private:
	std::map<std::string, wrl::ComPtr<ID3D11Texture2D>> textures;

};

