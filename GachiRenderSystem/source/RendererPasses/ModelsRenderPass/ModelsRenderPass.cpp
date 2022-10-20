#include "ModelsRenderPass.h"

#include "ResourceManagers/States/Samplers.h"
using namespace Renderer;
ModelsRenderPass::DrawCall::DrawCall(ModelsManager::SavedModel model, TexturesManager::TextureCache texture, Transform position, size_t flags) :
    model(model), texture(texture), position(position), flags(flags) {}

ModelsRenderPass::DrawLerpCall::DrawLerpCall(ModelsManager::SavedModel model, TexturesManager::TextureCache texture, const LerpModelDrawData& data) :
    model(model), texture(texture), data(data) {}


ModelsRenderPass::ModelsRenderPass(BaseRenderSystem& renderSystem) : BaseRenderPass({"ModelsShader.hlsl",renderSystem})
{
    int32_t width, height;
    renderDevice->GetBackbufferSize(&width, &height);
    pDataCB = renderDevice->CreateConstBuffer(sizeof(dataBuffer));

    vp.x = 0;
    vp.y = 0;
    vp.w = width;
    vp.h = height;
    vp.minDepth = 0.0f;
    vp.maxDepth = 1.0f;
}

void ModelsRenderPass::Draw(ModelsManager::SavedModel model, TexturesManager::TextureCache texture, Transform position, size_t flags)
{
    drawCalls.emplace_back(model, texture, position, flags);
}

void ModelsRenderPass::DrawLerp(ModelsManager::SavedModel model, TexturesManager::TextureCache texture, const LerpModelDrawData& data)
{
    drawLerpCalls.emplace_back(model, texture, data);
}


void ModelsRenderPass::Init(const char* dirr)
{
    BaseRenderPass::Init(dirr, new ModelsPassProvider());
}

void ModelsRenderPass::PreRender()
{
    RenderTargetBinding* targets[5] = {
        baseRendererParams.renderSystem.texturesManger->GetRenderTarget(SID("outTexture")),
        baseRendererParams.renderSystem.texturesManger->GetRenderTarget(SID("lightColor")),
        baseRendererParams.renderSystem.texturesManger->GetRenderTarget(SID("velocityField")),
        baseRendererParams.renderSystem.texturesManger->GetRenderTarget(SID("blurMask")),
        baseRendererParams.renderSystem.texturesManger->GetRenderTarget(SID("normalsField")),
    };

    renderDevice->SetRenderTargets(targets, std::size(targets), baseRendererParams.renderSystem.texturesManger->depthBuffer, vp);
    renderDevice->Clear(CLEAROPTIONS_TARGET, {}, 0, 0);
    renderDevice->Clear(CLEAROPTIONS_DEPTHBUFFER, {}, 0, 0);
}

void ModelsRenderPass::Render()
{
    int32_t width, height;
    renderDevice->GetBackbufferSize(&width, &height);

    RenderTargetBinding* targets[5] = {
        baseRendererParams.renderSystem.texturesManger->GetRenderTarget(SID("outTexture")),
        baseRendererParams.renderSystem.texturesManger->GetRenderTarget(SID("lightColor")),
        baseRendererParams.renderSystem.texturesManger->GetRenderTarget(SID("velocityField")),
        baseRendererParams.renderSystem.texturesManger->GetRenderTarget(SID("blurMask")),
        baseRendererParams.renderSystem.texturesManger->GetRenderTarget(SID("normalsField")),
    };

    renderDevice->SetRenderTargets(targets, std::size(targets), baseRendererParams.renderSystem.texturesManger->depthBuffer, vp);


    size_t lastFlags = -1;

    renderDevice->VerifyConstBuffer(pDataCB, modelsCosntBuffer.slot);


    renderDevice->VerifyPixelSampler(0, Samplers::anisotropic16);


    for (size_t i = 0; i < drawCalls.size(); i++)
    {
        if (drawCalls[i].flags != lastFlags)
        {
            renderDevice->ApplyPipelineState(factory->GetState(drawCalls[i].flags));
            lastFlags = drawCalls[i].flags;
        }

        renderDevice->ApplyVertexBufferBinding(drawCalls[i].model.vertexBuffer);
        renderDevice->ApplyIndexBufferBinding(drawCalls[i].model.indexBuffer, drawCalls[i].model.indexBufferElementSize);


        dataBuffer.alpha = 1;
        dataBuffer.oldAlpha = 1;
        dataBuffer.wh = float2(drawCalls[i].texture.width, drawCalls[i].texture.height);
        dataBuffer.color = { 1,0,1 };
        dataBuffer.world = drawCalls[i].position.GetTransform();
        dataBuffer.oldWorld = drawCalls[i].position.GetTransform();
        dataBuffer.blurSwitch = 2.f;
        //if (drawCalls[i].data.isGun)
        //	dataBuffer.blurSwitch = 1.f;
        renderDevice->SetConstBuffer(pDataCB, &dataBuffer);

        renderDevice->DrawIndexedPrimitives(
            drawCalls[i].model.pt, 0, 0, 0, 0,
            drawCalls[i].model.primitiveCount);
    }

    for (size_t i = 0; i < drawLerpCalls.size(); i++)
    {
        if (drawLerpCalls[i].data.flags != lastFlags)
        {
            renderDevice->ApplyPipelineState(factory->GetState(drawLerpCalls[i].data.flags));
            lastFlags = drawLerpCalls[i].data.flags;
        }


        static Buffer* vertexBuffers[3];
        static UINT ofsets[3] = {0,0,0};
        static UINT strides[3] = {0,0,0};
        static VertexBufferBinding vBB;

        vBB.buffersCount = 3;
        if (drawLerpCalls[i].data.isSingle)
            vBB.buffersCount = 2;
        vBB.vertexBuffers = vertexBuffers;
        int max_buff = drawLerpCalls[i].model.vertexBuffer.buffersCount - 1;
        vertexBuffers[0] = drawLerpCalls[i].model.vertexBuffer.vertexBuffers[0];
        vertexBuffers[1] = drawLerpCalls[i].model.vertexBuffer.vertexBuffers[drawLerpCalls[i].data.currentFrame % max_buff + 1];
        vertexBuffers[2] = drawLerpCalls[i].model.vertexBuffer.vertexBuffers[drawLerpCalls[i].data.nextFrame % max_buff + 1];

        strides[0] = drawLerpCalls[i].model.vertexBuffer.vertexStride[0];
        strides[1] = drawLerpCalls[i].model.vertexBuffer.vertexStride[drawLerpCalls[i].data.currentFrame + 1];
        strides[2] = drawLerpCalls[i].model.vertexBuffer.vertexStride[drawLerpCalls[i].data.nextFrame + 1];

        vBB.vertexOffset = ofsets;
        vBB.vertexStride = strides;

        renderDevice->ApplyVertexBufferBinding(vBB);
        renderDevice->ApplyIndexBufferBinding(drawLerpCalls[i].model.indexBuffer, drawLerpCalls[i].model.indexBufferElementSize);


        dataBuffer.alpha = drawLerpCalls[i].data.alpha;
        dataBuffer.oldAlpha = drawLerpCalls[i].data.oldAlpha;
        dataBuffer.wh = float2(drawLerpCalls[i].texture.width, drawLerpCalls[i].texture.height);
        dataBuffer.color = drawLerpCalls[i].data.color;
        dataBuffer.world = drawLerpCalls[i].data.newPosition.GetTransform();
        dataBuffer.oldWorld = drawLerpCalls[i].data.oldPosition.GetTransform();
        dataBuffer.blurSwitch = 2.f;
        if (drawLerpCalls[i].data.isGun)
            dataBuffer.blurSwitch = 1.f;
        renderDevice->SetConstBuffer(pDataCB, &dataBuffer);



        renderDevice->DrawIndexedPrimitives(
            drawLerpCalls[i].model.pt, 0, 0, 0, 0,
            drawLerpCalls[i].model.primitiveCount);
    }
}

void ModelsRenderPass::PostRender()
{
    drawCalls.clear();
    drawLerpCalls.clear();
}

ModelsRenderPass::~ModelsRenderPass()
{
    renderDevice->AddDisposeConstBuffer(pDataCB);
}
