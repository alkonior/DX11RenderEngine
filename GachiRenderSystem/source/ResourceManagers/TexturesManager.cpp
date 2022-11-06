#include "TexturesManager.h"

using namespace Renderer;

TexturesManager::TexturesManager(Renderer::IRenderer* renderDevice) : ITexturesManager(renderDevice) 
{
    uint32_t width, height;
    renderDevice->GetMainViewportSize(width, height);

    depthBuffer = renderDevice->GenDepthStencilRenderbuffer(width, height, DepthFormat::DEPTHFORMAT_D32, 0);

    CreateRenderTarget(("diffuseColor"),  SURFACEFORMAT_COLOR,         false, true, false,  width, height,          diffuseColor, diffuseColorRT);
    CreateRenderTarget(("lightColor"),    SURFACEFORMAT_COLOR,         false, true, false,  width, height,          lightColor, lightColorRT);
    CreateRenderTarget(("bloomMask"),     SURFACEFORMAT_COLOR,         false, true, false,  width, height,          bloomMask, bloomMaskRT);
    CreateRenderTarget(("alphaSurfaces"), SURFACEFORMAT_COLOR,         false, true, false,  width, height,          alphaSurfaces, alphaSurfacesRT);
    CreateRenderTarget(("preAAcolor"),    SURFACEFORMAT_COLOR,         false, true, false,  width, height,          preAAcolor, preAAcolorRT);
    CreateRenderTarget(("velocityField"), SURFACEFORMAT_HALFVECTOR4,   false, true, false,  width, height,          velocityField, velocityFieldRT);
    CreateRenderTarget(("blurMask"),      SURFACEFORMAT_SINGLE,        false, true, false,  width, height,          blurMask, blurMaskRT);;
    CreateRenderTarget(("normalsField"),  SURFACEFORMAT_VECTOR4,       false, true, false,  width, height,          normalsField, normalsFieldRT);
    CreateRenderTarget(("oclusionField"),       SURFACEFORMAT_SINGLE,  true,  true, false,  width, height, oclusionField, oclusionFieldRT);

    CreateRenderTarget(("pastColor"),     SURFACEFORMAT_COLOR,         false, true, false,  width, height,          pastColor, pastColorRT);
    CreateRenderTarget(("pastDepth"),     SURFACEFORMAT_SINGLE,        false, true, false,  width, height,          pastDepth, pastDepthRT);

    CreateRenderTarget(("outTexture"),    SURFACEFORMAT_COLOR,         false, true, false,  width, height,          normalsField, normalsFieldRT);


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
void TexturesManager::CreateRenderTarget(const char* name, SurfaceFormat format,
    bool isUA,
    bool isVS,
    bool isSS,
    uint32_t width, uint32_t height, Renderer::Texture*& texture,
    Renderer::RenderTargetBinding& renderTarget)
{
    renderTarget = CreatePublicRenderTarget({
        name,
        format,
        isUA,
        isVS,
        isSS,
        width,height
    });
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
const Renderer::RenderTargetBinding& TexturesManager::CreatePublicRenderTarget(const RenderTargetDescription& description)
{
    auto id = SIDRT(description.name);
    assert(!publicRenderTargets.contains(id));

    Texture* texture = nullptr;
    uint32_t width, height;
    width = description.width;
    height = description.height;
    //if (description.isSS)
    //    renderDevice->GetBackbufferSize(width, height);//todo
    if (description.isVS)
        renderDevice->GetMainViewportSize(width, height);

    if (description.isUA)
        texture = renderDevice->CreateUATexture2D(description.format, width, height, 1);
    else
        texture = renderDevice->CreateTexture2D(description.format, width, height, 1, true);
    
    publicRenderTargetsDescriptions[id] = description;
    publicRenderTargetsDescriptions[id].width = width;
    publicRenderTargetsDescriptions[id].height = height;
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


void TexturesManager::ResizeTextures()
{
    uint32_t vswidth, vsheight;
    uint32_t sswidth, ssheight;
    renderDevice->GetMainViewportSize(vswidth, vsheight);
    renderDevice->GetMainViewportSize(sswidth, ssheight);
    for (auto&[id, descritpion] : publicRenderTargetsDescriptions)
    {
        if (descritpion.isSS)
        {
            if (descritpion.width != sswidth || descritpion.height != ssheight)
            {
                ReleasePublicRenderTarget(id);
                CreatePublicRenderTarget(publicRenderTargetsDescriptions[id]);
            }
        }
        if (descritpion.isVS)
        {
            if (descritpion.width != vswidth || descritpion.height != vsheight)
            {
                ReleasePublicRenderTarget(id);
                CreatePublicRenderTarget(publicRenderTargetsDescriptions[id]);
            }
        }
    }
}


const RenderTargetBinding& TexturesManager::CreatePrivateRenderTarget(
    const RenderTargetDescription& description,
    Texture*& pTexture,
    RenderTargetBinding& pRT)
{
    auto id = SIDRT(description.name);
    assert(!privateRenderTargets.contains(id));
    uint32_t width, height;
    width = description.width;
    height = description.height;
    //if (description.isSS)
    //    renderDevice->GetBackbufferSize(width, height);//todo
    if (description.isVS)
        renderDevice->GetMainViewportSize(width, height);

    if (description.isUA)
        pTexture = renderDevice->CreateUATexture2D(description.format, width, height, 1);
    else
        pTexture = renderDevice->CreateTexture2D(description.format, width, height, 1, true);

    privateRenderTargets[id] =
    {
        1,0,
        diffuseColor,
        nullptr,
        {
            0,0,(int32_t)width,(int32_t)height,0.0,1.0
        }
    };
    pRT = privateRenderTargets[id];
    return pRT;
}
Renderer::RenderTargetBinding* TexturesManager::GetRenderTarget(Renderer::string_id id)
{
    return &publicRenderTargets[id];
}

ITexturesManager::RenderTargetDescription TexturesManager::GetRenderTargetDescription(Renderer::string_id id)
{
    return publicRenderTargetsDescriptions[id];
}

std::vector<const char*> TexturesManager::GetRenderTargetsList()
{
    std::vector<const char*> result;
    for (auto& [id, desc] : publicRenderTargetsDescriptions)
    {
        result.push_back(desc.name);
    }
    return result;
}

void TexturesManager::ReleasePublicRenderTarget(Renderer::string_id id)
{
    renderDevice->AddDisposeTexture(publicRenderTargets[id].texture);
    publicRenderTargets.erase(id);
}


TexturesManager::TextureCache TexturesManager::GetImg(size_t id)
{
    if (textures.contains(id))
        return textures[id];
    return {};
}
