#include "TexturesManager.h"

TexturesManager::TexturesManager() {}

void TexturesManager::RegTexture(GraphicsBase& gfx,const MyTexture& tx, size_t id) {
	HRESULT hr;
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = tx.GetWidth();
	textureDesc.Height = tx.GetHeight();
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DYNAMIC;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	textureDesc.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = tx.GetBufferPtr();
	sd.SysMemPitch = tx.GetWidth() *sizeof(MyTexture::Color);

	auto&  pTexture = textures[id];
	//GFX_THROW_INFO(gfx.pDevice->CreateTexture2D(&textureDesc, &sd, &pTexture.texture));
	pTexture.width = tx.GetWidth();
	pTexture.height = tx.GetHeight();
}

void TexturesManager::ReeaseTexture(size_t id) {
	textures.erase(id);
}

TexturesManager::TextureCache TexturesManager::GetImg(GraphicsBase& gfx, size_t id) {
	return textures[id];
	
}



