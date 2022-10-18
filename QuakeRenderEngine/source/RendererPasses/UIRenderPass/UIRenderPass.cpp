#include "UIRenderPass.h"

#include "RenderFlags.h"
#include "UIRenderPassProvider.h"
#include "ResourceManagers/States/Samplers.h"


using namespace Renderer;


UIRenderPass::UIRenderPass(BaseRenderSystem& renderSystem) : BaseRenderPass<>({"UIShader.hlsl", renderSystem}) {

    //renderDevice = in.renderSystem.pRenderer;
    int32_t width, height;
    renderDevice->GetBackbufferSize(&width, &height);

    Vertex2D vertices[] =
    {
        {  float2(-1.0f, 1.0f),  float2(0.0f, 1.0f), },
        {  float2(-1.0f, 0.0f),  float2(0.0f, 0.0f), },
        {  float2(0.0f,  1.0f),  float2(1.0f, 1.0f), },
        {  float2(0.0f,  0.0f),  float2(1.0f, 0.0f), }
    };
    vertexBuffer.buffersCount = 1;
    vertexBuffer.vertexBuffers = new Buffer * [1]();
    vertexBuffer.vertexBuffers[0] = renderDevice->GenVertexBuffer(0, BufferUsage::BUFFERUSAGE_NONE, sizeof(vertices));
    renderDevice->SetVertexBufferData(vertexBuffer.vertexBuffers[0], 0, &vertices, 4, sizeof(Vertex2D), sizeof(Vertex2D), SetDataOptions::SETDATAOPTIONS_NONE);
    //GFX_THROW_INFO(gfx.pDevice->CreateBuffer(&bd, &sd, &pVertexBuffer));
    vertexBuffer.vertexOffset = new (UINT)(0);
    vertexBuffer.vertexStride = new (UINT)(sizeof(Vertex2D));
    // Bind vertex buffer to pipeline


    // create index buffer
    const uint16_t indices[] =
    {
        0,1,2,
        1,3,2
    };
    indexBuffer = renderDevice->GenIndexBuffer(0, BufferUsage::BUFFERUSAGE_WRITEONLY, 12);
    renderDevice->SetIndexBufferData(indexBuffer, 0, (void*)indices, 12, SetDataOptions::SETDATAOPTIONS_DISCARD);



    vp.x = 0;
    vp.y = 0;
    vp.w = width;
    vp.h = height;
    vp.minDepth = 0.0f;
    vp.maxDepth = 1.0f;

    constBuffer = renderDevice->CreateConstBuffer(sizeof(localBuffer));


    sampler.filter = TextureFilter::TEXTUREFILTER_POINT;
    sampler.addressU = TextureAddressMode::TEXTUREADDRESSMODE_WRAP;
    sampler.addressV = TextureAddressMode::TEXTUREADDRESSMODE_WRAP;
    sampler.addressW = TextureAddressMode::TEXTUREADDRESSMODE_WRAP;


}

UIRenderPass::DrawCall::DrawCall(TexturesManager::TextureCache texture, const UIDrawData& data) :data(data), texture(texture) {}
UIRenderPass::DrawCall::DrawCall(const UIDrawData& data) : data(data) {}
matrix UIRenderPass::DrawCall::getTransform(size_t screenW, size_t screenH)  {
    return
        matrix::CreateScale(data.width * 1.0f, data.height * 1.0f, 0) *
        matrix::CreateTranslation(data.x * 1.0f, data.y * 1.0f, 0) *
        matrix::CreateScale(2.0f / screenW, 2.0f / screenH, 0) *
        matrix::CreateTranslation(-1, -1, 0) *
        matrix::CreateScale(1, -1, 0) *
        matrix::CreateTranslation(data.width * 2.0f / screenW, 0, 0);
}
float2 UIRenderPass::DrawCall::getUVShift() {
	return float2(data.top * 1.0f / texture.width, data.left * 1.0f / texture.height);
}
float2 UIRenderPass::DrawCall::getUVScale()
{
	return float2(data.texW * 1.0f / texture.width, data.texH * 1.0f / texture.height);
}


void UIRenderPass::Init(const char* dirr)
{
    BaseRenderPass<>::Init(dirr, new UIRenderPassProvider());
}

void UIRenderPass::PreRender()
{
}


void UIRenderPass::Render()
{
    int32_t width, height;
    renderDevice->GetBackbufferSize(&width, &height);
    uint64_t lastFlag = -1;
    //renderDevice->ApplyPipelineState(factory->GetState(0));

    renderDevice->ApplyVertexBufferBinding(vertexBuffer);
    renderDevice->ApplyIndexBufferBinding(indexBuffer, 16);
    renderDevice->VerifyPixelSampler(0, Samplers::pointClamp);

    renderDevice->SetRenderTargets(nullptr, 0, nullptr, vp);
    renderDevice->VerifyConstBuffer(constBuffer, uiTransformCB.slot);

    for (size_t i = 0; i < drawCalls.size(); i++) {
        if (drawCalls[i].data.flag != lastFlag) {
            if (drawCalls[i].data.flag & UICHAR) { renderDevice->BeginEvent("Chars"); }
            if (lastFlag & UICHAR) { renderDevice->EndEvent(); }
            renderDevice->ApplyPipelineState(factory->GetState(drawCalls[i].data.flag));
            lastFlag = drawCalls[i].data.flag;
        }

        localBuffer.transform = drawCalls[i].getTransform(width, height).Transpose();
        localBuffer.uvShift = drawCalls[i].getUVShift();
        localBuffer.uvScale = drawCalls[i].getUVScale();
        localBuffer.color = drawCalls[i].data.color;

        renderDevice->SetConstBuffer(constBuffer, &localBuffer);

        auto  pTexture = drawCalls[i].texture.texture;
        renderDevice->VerifyPixelTexture(0, pTexture);


        renderDevice->DrawIndexedPrimitives(Renderer::PrimitiveType::PRIMITIVETYPE_TRIANGLELIST,
            0, 0, 0, 0, 2);

    }
    if (lastFlag & UICHAR) { renderDevice->EndEvent(); }

    
}
void UIRenderPass::PostRender()
{
    drawCalls.clear();
}


void UIRenderPass::Draw(TexturesManager::TextureCache texture, const UIDrawData& data)
{
	drawCalls.push_back(DrawCall(texture, data));
}
void UIRenderPass::Draw(const UIDrawData& data)
{
	drawCalls.push_back(DrawCall(data));    
}




UIRenderPass::~UIRenderPass()
{
    delete vertexBuffer.vertexOffset;
    delete vertexBuffer.vertexStride;
    renderDevice->AddDisposeVertexBuffer(vertexBuffer.vertexBuffers[0]);
    delete[] vertexBuffer.vertexBuffers;
    renderDevice->AddDisposeIndexBuffer(indexBuffer);
    renderDevice->AddDisposeConstBuffer(constBuffer);
    //delete provider;
    delete factory;
    
}

