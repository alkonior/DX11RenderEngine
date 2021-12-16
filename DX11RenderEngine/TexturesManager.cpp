#include "TexturesManager.h"

TexturesManager::TexturesManager() {}

void TexturesManager::RegTexture(GraphicsBase& gfx,const Texture& tx, const char* name) {
	HRESULT hr;
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = tx.GetWidth();
	textureDesc.Height = tx.GetHeight();
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = tx.GetBufferPtr();
	sd.SysMemPitch = tx.GetWidth() *sizeof(Texture::Color);

	auto&  pTexture = textures[std::string(name)];
	GFX_THROW_INFO(gfx.pDevice->CreateTexture2D(&textureDesc, &sd, &pTexture.texture));
	pTexture.width = tx.GetWidth();
	pTexture.height = tx.GetHeight();
}

TexturesManager::TextureCache TexturesManager::GetImg(GraphicsBase& gfx, const char* name) {
	return textures[std::string(name)];;
	
}



