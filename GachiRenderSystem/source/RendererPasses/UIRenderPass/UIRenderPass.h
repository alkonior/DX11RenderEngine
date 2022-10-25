#pragma once
#include "CoreRenderSystem/RenderPasses/QuadRenderPass.h"
#include "UIRenderPassConstBuffer.h"
#include "ResourceManagers/TexturesManager.h"
#include "Utils/DrawData.h"


class UIRenderPass : public BaseRenderPass {

    
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
    
    PipelineFactoryFlags ParseFlags(size_t flag);
public:
    explicit UIRenderPass(BaseRenderSystem& in);


    
    void Init(const char* dirr) override;
    
    void PreRender() override;

    void Render();
    
    void PostRender() override;


    void Draw(TexturesManager::TextureCache texture, const UIDrawData& data);
    void Draw(const UIDrawData& data);
    
    ~UIRenderPass() override;
private:
    Renderer::Viewport vp;

    Renderer::VertexBufferBinding vertexBuffer;
    Renderer::Buffer* indexBuffer;
    UIRenderPassConstBuffer localBuffer;
    Renderer::ConstBuffer* constBuffer;

    Renderer::SamplerState sampler;
    std::vector<DrawCall> drawCalls;
    

};
