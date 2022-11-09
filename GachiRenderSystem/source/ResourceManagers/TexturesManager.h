#pragma once

#include <map>
#include <unordered_map>

#include "CoreRenderSystem/ResourceManagers/ITexturesManager.h"

class TexturesManager : public ITexturesManager
{
public:
public:
    TexturesManager(Renderer::IRenderer* renderDevice);

    virtual void RegTexture(void* tx, int width, int height, size_t id);
    virtual void RegTexture(const TextureData& tx, size_t id);
    virtual void UpdateFloatTexture(const TextureData& tx, size_t id);
    virtual void UpdateFloatTexture(const ImageUpdate& updateData);
    virtual void ResizeTextures() override; 

    uint32_t depthWidth;
    uint32_t depthHeigh;
    
    virtual void ReleaseTexture(size_t id);
    virtual TextureCache GetImg(size_t id);
    //void RegImgFromFile(GraphicsBase gfx, size_t width, size_t height, LPCSTR file, LPCSTR name);

    //RenderTeargets::

    //Renderer::Renderbuffer* depthBuffer;


private:
    void CreateRenderTarget(const char* name,
                            Renderer::SurfaceFormat format,
                            bool isUA,
                            bool isVS,
                            bool isSS,
                            uint32_t width, uint32_t height);
    //
public:
    virtual ~TexturesManager();

    virtual const Renderer::RenderTargetBinding& CreatePublicRenderTarget(
        const RenderTargetDescription& description) override;

    virtual const Renderer::RenderTargetBinding& CreatePrivateRenderTarget(
        const RenderTargetDescription& description,
        Renderer::Texture*& pTexture,
        Renderer::RenderTargetBinding& pRT
    ) override;

    virtual Renderer::RenderTargetBinding* GetRenderTarget(Renderer::string_id id) override;
    virtual RenderTargetDescription GetRenderTargetDescription(Renderer::string_id id) override;

    std::vector<const char*> GetRenderTargetsList() override;
private:
    std::map<Renderer::string_id, TextureCache> textures;
    std::map<Renderer::string_id, FloatTextureCache> floatTextures;
    std::map<Renderer::string_id, Float3TextureCache> float3Textures;
    
    void ReleasePublicRenderTarget(Renderer::string_id id);
public:

#pragma region FloatTextres
    
    void RegFloatTexture(float* tx, int width, int height, size_t id) override;
    void RegFloatTexture(const FloatData& tx, size_t id) override;
    void UpdateFloatTexture(const FloatData& tx, size_t id) override;
    void UpdateFloatTexture(const FloatImageUpdate& updateData) override;
    void ReleaseFloatTexture(size_t id) override;
    FloatTextureCache GetFloatImg(size_t id) override;

    
#pragma endregion 

#pragma region Float3Textres
    
    void RegFloat3Texture(float3* tx, int width, int height, size_t id) override;
    void RegFloat3Texture(const Float3Data& tx, size_t id) override;
    void UpdateFloat3Texture(const Float3Data& tx, size_t id) override;
    void UpdateFloat3Texture(const Float3ImageUpdate& updateData) override;
    void ReleaseFloat3Texture(size_t id) override;
    Float3TextureCache GetFloat3Img(size_t id) override;
    
#pragma endregion 
    
private:
    std::unordered_map<Renderer::string_id, Renderer::RenderTargetBinding> publicRenderTargets;
    std::unordered_map<Renderer::string_id, Renderer::RenderTargetBinding> privateRenderTargets;
    std::unordered_map<Renderer::string_id, RenderTargetDescription> publicRenderTargetsDescriptions;
};

