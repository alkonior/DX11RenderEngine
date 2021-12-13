#include "TexturesManager.h"

void TexturesManager::RegTexture(GraphicsBase gfx, Texture& tx, char* name) {
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
	sd.SysMemPitch = tx.GetWidth()* tx.GetHeight() *sizeof(Texture::Color);

	auto  pTexture = textures[std::string(name)];
	GFX_THROW_INFO(gfx.pDevice->CreateTexture2D(
		&textureDesc, &sd, &pTexture
	));
}

wrl::ComPtr<ID3D11ShaderResourceView> TexturesManager::BindImg(GraphicsBase gfx, char* name) {
	HRESULT hr;

	auto  pTexture = textures[std::string(name)];
	// create the resource view on the texture
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	wrl::ComPtr<ID3D11ShaderResourceView> pTextureView;
	GFX_THROW_INFO(gfx.pDevice->CreateShaderResourceView(
		pTexture.Get(), &srvDesc, &pTextureView
	));
	return pTextureView;
}



