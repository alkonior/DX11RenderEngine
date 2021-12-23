#include "Manager2D.h"
using namespace DirectX::SimpleMath;

Manager2D::Manager2D(GraphicsBase& gfx) {
	HRESULT hr;

	Vertex2D vertices[ ] =
	{
		{  Vector2(-1.0f, 1.0f),  Vector2(0.0f, 1.0f), },
		{  Vector2(-1.0f, 0.0f),  Vector2(0.0f, 0.0f), },
		{  Vector2( 0.0f, 1.0f),  Vector2(1.0f, 1.0f), },
		{  Vector2( 0.0f, 0.0f),  Vector2(1.0f, 0.0f), }
	};

	D3D11_BUFFER_DESC bd = {};
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0u;
	bd.MiscFlags = 0u;
	bd.ByteWidth = sizeof(vertices);
	bd.StructureByteStride = sizeof(Vertex2D);
	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = vertices;
	GFX_THROW_INFO(gfx.pDevice->CreateBuffer(&bd, &sd, &pVertexBuffer));

	// Bind vertex buffer to pipeline


	// create index buffer
	const unsigned short indices[ ] =
	{
		0,1,2,
		1,3,2
	};
	D3D11_BUFFER_DESC ibd = {};
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.CPUAccessFlags = 0u;
	ibd.MiscFlags = 0u;
	ibd.ByteWidth = sizeof(indices);
	ibd.StructureByteStride = sizeof(unsigned short);
	D3D11_SUBRESOURCE_DATA isd = {};
	isd.pSysMem = indices;
	GFX_THROW_INFO(gfx.pDevice->CreateBuffer(&ibd, &isd, &pIndexBuffer));


	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	GFX_THROW_INFO(gfx.pDevice->CreateSamplerState(&samplerDesc, &pSampler));

}

void Manager2D::Draw(TexturesManager::TextureCache texture, size_t top, size_t left, size_t texW, size_t texH, size_t x, size_t y, size_t width, size_t height) {
	drawCalls.emplace_back(texture, top, left, texH, texW, x, y, width, height);
}

void Manager2D::Draw(TexturesManager::TextureCache texture, size_t x, size_t y, size_t width, size_t height) {
	drawCalls.emplace_back(texture, x, y, width, height);
}

void Manager2D::Present(GraphicsBase& gfx) {

	const UINT stride = sizeof(Vertex2D);
	const UINT offset = 0u;
	gfx.pContext->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);

	gfx.pContext->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);

	PixelShader2D::Bind(gfx);
	VertexShader2D::Bind(gfx);

	gfx.pContext->OMSetRenderTargets(1u, gfx.pTarget.GetAddressOf(), nullptr);

	gfx.pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	gfx.pContext->PSSetSamplers(0, 1, pSampler.GetAddressOf());



	HRESULT hr;


	// create the resource view on the texture
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	wrl::ComPtr<ID3D11ShaderResourceView> pTextureView;


	for (size_t i = 0; i < drawCalls.size(); i++) {
		auto  pTexture = drawCalls[i].texture.texture;

		GFX_THROW_INFO(gfx.pDevice->CreateShaderResourceView(pTexture.Get(), &srvDesc, &pTextureView));
		GFX_THROW_INFO_ONLY(gfx.pContext->PSSetShaderResources(0, 1u, pTextureView.GetAddressOf()));

		VertexShader2D::SetTransform(drawCalls[i].getTransform(gfx.width, gfx.height), drawCalls[i].getUVShift(), drawCalls[i].getUVScale());
		VertexShader2D::UpdateConstBuff(gfx);



		GFX_THROW_INFO_ONLY(gfx.pContext->DrawIndexed(6u, 0u, 0u));
	}
	drawCalls.clear();

}

Manager2D::~Manager2D() {}

Manager2D::Rectangle::Rectangle(TexturesManager::TextureCache texture, size_t top, size_t left, size_t texW, size_t texH, size_t x, size_t y, size_t width, size_t height)
	:x(x), y(y), width(width), height(height), texture(texture), top(top), left(left), texH(texH), texW(texW) {}

Manager2D::Rectangle::Rectangle(TexturesManager::TextureCache texture, size_t x, size_t y, size_t width, size_t height)
	:x(x), y(y), width(width), height(height), texture(texture), top(0), left(0), texH(texture.height), texW(texture.width) {}

dx::SimpleMath::Matrix Manager2D::Rectangle::getTransform(size_t screenW, size_t screenH) {
	return

		Matrix::CreateScale(width*1.0f, height * 1.0f, 0) *
		Matrix::CreateTranslation(x * 1.0f, y * 1.0f, 0) *
		Matrix::CreateScale(2.0f / screenW, 2.0f / screenH, 0) *
		Matrix::CreateTranslation(-1, -1, 0) *
		Matrix::CreateScale(1, -1, 0) *
		Matrix::CreateTranslation(width * 2.0f / screenW, 0, 0);
}

dx::SimpleMath::Vector2 Manager2D::Rectangle::getUVShift() {
	return dx::SimpleMath::Vector2(top*1.0f/texture.width, left*1.0f/texture.height);
}

dx::SimpleMath::Vector2 Manager2D::Rectangle::getUVScale() {
	return dx::SimpleMath::Vector2(texW * 1.0f / texture.width, texH * 1.0f / texture.height);
}
