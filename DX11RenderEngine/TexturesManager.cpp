#include "TexturesManager.h"


TexturesManager::TexturesManager(Renderer::IRenderer* renderer):renderer(renderer) {}

void TexturesManager::RegTexture(const TextureData& tx, size_t id) {
	//HRESULT hr;
	//D3D11_TEXTURE2D_DESC textureDesc = {};
	//textureDesc.Width = tx.GetWidth();
	//textureDesc.Height = tx.GetHeight();
	//textureDesc.MipLevels = 1;
	//textureDesc.ArraySize = 1;
	//textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	//textureDesc.SampleDesc.Count = 1;
	//textureDesc.SampleDesc.Quality = 0;
	//textureDesc.Usage = D3D11_USAGE_DYNAMIC;
	//textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	//textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	//textureDesc.MiscFlags = 0;
	//D3D11_SUBRESOURCE_DATA sd = {};
	//sd.pSysMem = tx.GetBufferPtr();
	//sd.SysMemPitch = tx.GetWidth() *sizeof(TextureData::Color);

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



