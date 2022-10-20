#include "TexturesManager.h"

using namespace Renderer;

TexturesManager::TexturesManager(Renderer::IRenderer* renderDevice) : ITexturesManager(renderDevice) 
{
    int width, height;
    renderDevice->GetBackbufferSize(&width, &height);

    depthBuffer = renderDevice->GenDepthStencilRenderbuffer(width, height, DepthFormat::DEPTHFORMAT_D32, 0);

    CreateRenderTarget(SID("diffuseColor"),  SURFACEFORMAT_COLOR,         false, width, height,          diffuseColor, diffuseColorRT);
    CreateRenderTarget(SID("lightColor"),    SURFACEFORMAT_COLOR,         false, width, height,          lightColor, lightColorRT);
    CreateRenderTarget(SID("bloomMask"),     SURFACEFORMAT_COLOR,         false, width, height,          bloomMask, bloomMaskRT);
    CreateRenderTarget(SID("alphaSurfaces"), SURFACEFORMAT_COLOR,         false, width, height,          alphaSurfaces, alphaSurfacesRT);
    CreateRenderTarget(SID("preAAcolor"),    SURFACEFORMAT_COLOR,         false, width, height,          preAAcolor, preAAcolorRT);
    CreateRenderTarget(SID("velocityField"), SURFACEFORMAT_RGBA1010102,   false, width, height,          velocityField, velocityFieldRT);
    CreateRenderTarget(SID("blurMask"),      SURFACEFORMAT_SINGLE,        false, width, height,          blurMask, blurMaskRT);;
    CreateRenderTarget(SID("pastColor"),     SURFACEFORMAT_COLOR,         false, width, height,          pastColor, pastColorRT);
    CreateRenderTarget(SID("pastDepth"),     SURFACEFORMAT_SINGLE,        false, width, height,          pastDepth, pastDepthRT);
    CreateRenderTarget(SID("normalsField"),  SURFACEFORMAT_VECTOR4,       false, width, height,          normalsField, normalsFieldRT);

    CreateRenderTarget(SID("outTexture"),    SURFACEFORMAT_COLOR,       false, width, height,          normalsField, normalsFieldRT);


    CreateRenderTarget(SID("oclusionField"),       SURFACEFORMAT_SINGLE, true, width, height, oclusionField, oclusionFieldRT);
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
void TexturesManager::CreateRenderTarget(string_id name, SurfaceFormat format, bool isUA, size_t width, size_t height, Renderer::Texture*& texture,
    Renderer::RenderTargetBinding& renderTarget)
{
    renderTarget = CreatePublicRenderTarget(name, {format, isUA,width,height});
    texture = renderTarget.texture;
}

TexturesManager::~TexturesManager()
{
    for (auto& [key, texture] : textures)
    {    auto& pTexture = textures[key];
        if (pTexture.texture != nullptr)
            renderDevice->AddDisposeTexture(pTexture.texture);
        pTexture.texture = nullptr;
    }
    
    for (auto& [key, texture] : publicRenderTargets)
    {
        renderDevice->AddDisposeTexture(texture.texture);
    }
    
    for (auto& [key, texture] : privateRenderTargets)
    {
        renderDevice->AddDisposeTexture(texture.texture);
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
Renderer::RenderTargetBinding* TexturesManager::GetRenderTarget(Renderer::string_id id)
{
    return &publicRenderTargets[id];
}




TexturesManager::TextureCache TexturesManager::GetImg(size_t id)
{
    if (textures.contains(id))
        return textures[id];
    return {};
}
