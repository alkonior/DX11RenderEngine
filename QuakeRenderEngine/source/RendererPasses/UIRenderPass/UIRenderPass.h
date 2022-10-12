#pragma once
#include "CoreRenderSystem/RenderPasses/QuadRenderPass.h"
#include "RendererPasses/OldRenderPasses/UIRenderer/UIConstBuffers.h"
#include "ResourceManagers/TexturesManager.h"
#include "Utils/DrawData.h"


class UIRenderPass final : public BaseRenderPass<> {

    
    struct Vertex2D {
        float2 pos;
        float2 uv;
    };


    struct DrawCall {
        DrawCall(TexturesManager::TextureCache texture, const UIDrawData& data);
        DrawCall(const UIDrawData& data);

        matrix getTransform(size_t screenW, size_t screenH);
        float2 getUVShift();
        float2 getUVScale();

        TexturesManager::TextureCache texture;
        UIDrawData data;
    };
    
public:
    explicit UIRenderPass(const BaseRendererParams& in);


    
    void Init(const char* dirr) override;
    
    void PreRender() override;
    
    void Render() override;
    
    void PostRender() override;


    void Draw(TexturesManager::TextureCache texture, const UIDrawData& data);
    void Draw(const UIDrawData& data);
    
    ~UIRenderPass() override;
private:
    Renderer::Viewport vp;

    Renderer::VertexBufferBinding vertexBuffer;
    Renderer::Buffer* indexBuffer;
    Shader2DCosntBuffer localBuffer;
    Renderer::ConstBuffer* constBuffer;

    Renderer::SamplerState sampler;
    std::vector<DrawCall> drawCalls;
    

};
