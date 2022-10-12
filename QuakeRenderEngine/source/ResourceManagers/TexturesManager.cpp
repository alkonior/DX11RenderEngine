#include "TexturesManager.h"

using namespace Renderer;

TexturesManager::TexturesManager()
{
    int width, height;
    renderDevice->GetBackbufferSize(&width, &height);

    depthBuffer = renderDevice->GenDepthStencilRenderbuffer(width, height, DepthFormat::DEPTHFORMAT_D32, 0);

    CreateRenderTarget(SURFACEFORMAT_COLOR, width, height, diffuseColor, diffuseColorRT);
    CreateRenderTarget(SURFACEFORMAT_COLOR, width, height, lightColor, lightColorRT);
    CreateRenderTarget(SURFACEFORMAT_COLOR, width, height, bloomMask, bloomMaskRT);
    CreateRenderTarget(SURFACEFORMAT_COLOR, width, height, alphaSurfaces, alphaSurfacesRT);
    CreateRenderTarget(SURFACEFORMAT_COLOR, width, height, preAAcolor, preAAcolorRT);
    CreateRenderTarget(SURFACEFORMAT_RGBA1010102, width, height, velocityField, velocityFieldRT);
    CreateRenderTarget(SURFACEFORMAT_SINGLE, width, height, blurMask, blurMaskRT);;
    CreateRenderTarget(SURFACEFORMAT_COLOR, width, height, pastColor, pastColorRT);
    CreateRenderTarget(SURFACEFORMAT_SINGLE, width, height, pastDepth, pastDepthRT);
    CreateRenderTarget(SURFACEFORMAT_VECTOR4, width, height, normalsField, normalsFieldRT);

    CreateUATarget(SURFACEFORMAT_SINGLE, width, height, oclusionField, oclusionFieldRT);
}

void TexturesManager::RegTexture(void* data, int width, int height, bool mipmap, size_t id)
{
    auto& pTexture = textures[id];
    if (pTexture.texture != nullptr)
    {
        ReleaseTexture(id);
    }
    //GFX_THROW_INFO(gfx.pDevice->CreateTexture2D(&textureDesc, &sd, &pTexture.texture));
    pTexture.texture = renderDevice->CreateTexture2D(SurfaceFormat::SURFACEFORMAT_COLOR, width, height, 1, 0);
    renderDevice->SetTextureData2D(pTexture.texture, 0, 0, width, height, 0, data, width * height * 4);
    pTexture.width = width;
    pTexture.height = height;
}

void TexturesManager::RegTexture(const TextureData& tx, size_t id)
{
    auto& pTexture = textures[id];
    if (pTexture.texture != nullptr)
    {
        ReleaseTexture(id);
    }
    //GFX_THROW_INFO(gfx.pDevice->CreateTexture2D(&textureDesc, &sd, &pTexture.texture));
    pTexture.texture = renderDevice->CreateTexture2D(SurfaceFormat::SURFACEFORMAT_COLOR, tx.GetWidth(), tx.GetHeight(), 1, 0);
    renderDevice->SetTextureData2D(pTexture.texture, 0, 0, tx.GetWidth(), tx.GetHeight(), 0, (void*)tx.GetBufferPtr(), tx.GetWidth() * tx.GetHeight() * 4);
    pTexture.width = tx.GetWidth();
    pTexture.height = tx.GetHeight();
}

void TexturesManager::UpdateTexture(const TextureData& tx, size_t id)
{
    auto& pTexture = textures[id];
    if (pTexture.texture == nullptr)
    {
        RegTexture(tx, id);
    }
    else
    {
        renderDevice->SetTextureData2D(pTexture.texture, 0, 0, tx.GetWidth(), tx.GetHeight(), 0, (void*)tx.GetBufferPtr(), tx.GetWidth() * tx.GetHeight() * 4);
    }
}

void TexturesManager::UpdateTexture(const ImageUpdate& updateData)
{
    auto& pTexture = textures[updateData.id];
    if (pTexture.texture == nullptr)
    {
        pTexture.texture = renderDevice->CreateTexture2D(SURFACEFORMAT_COLOR, updateData.width, updateData.height, 1, 0);
        //return;
    }
    renderDevice->SetTextureData2D(pTexture.texture, updateData.box.x, updateData.box.y, updateData.box.w, updateData.box.h,
        updateData.level, updateData.data, 0);
}


void TexturesManager::ReleaseTexture(size_t id)
{
    auto& pTexture = textures[id];
    if (pTexture.texture != nullptr)
        renderDevice->AddDisposeTexture(pTexture.texture);
    pTexture.texture = nullptr;
    //textures.erase(id);
}

TexturesManager::~TexturesManager()
{
    for (auto& [key, texture] : textures)
    {
        ReleaseTexture(key);
    }
    
    for (auto& [key, texture] : publicRenderTargets)
    {
        ReleaseTexture(key);
    }
    
    for (auto& [key, texture] : privateRenderTargets)
    {
        ReleaseTexture(key);
    }

    renderDevice->AddDisposeRenderbuffer(depthBuffer);
}
const Renderer::RenderTargetBinding& TexturesManager::CreatePublicRenderTarget(string_id id, const RenderTargetDescription& description)
{
    assert(!publicRenderTargets.contains(id));

    Texture* texture = nullptr;
    if (description.isUA)
        texture = renderDevice->CreateUATexture2D(description.format, description.width, description.height, 1);
    else
        texture = renderDevice->CreateTexture2D(description.format, description.width, description.height, 1, true);

    publicRenderTargets[id] =
    {
        1,0,

        texture,
        nullptr,
        {
            0,0,(int32_t)description.width,(int32_t)description.height,0.0,1.0
        }
    };

    return publicRenderTargets[id];
}
const RenderTargetBinding& TexturesManager::CreatePrivateRenderTarget(string_id id,
    const RenderTargetDescription& description,
    Texture*& diffuseColor,
    RenderTargetBinding& diffuseColorRT)
{
    assert(!privateRenderTargets.contains(id));

    if (description.isUA)
        diffuseColor = renderDevice->CreateUATexture2D(description.format, description.width, description.height, 1);
    else
        diffuseColor = renderDevice->CreateTexture2D(description.format, description.width, description.height, 1, true);

    privateRenderTargets[id] =
    {
        1,0,
        diffuseColor,
        nullptr,
        {
            0,0,(int32_t)description.width,(int32_t)description.height,0.0,1.0
        }
    };
    diffuseColorRT = privateRenderTargets[id];
    
    return diffuseColorRT;
}




TexturesManager::TextureCache TexturesManager::GetImg(size_t id)
{
    return textures[id];
}
