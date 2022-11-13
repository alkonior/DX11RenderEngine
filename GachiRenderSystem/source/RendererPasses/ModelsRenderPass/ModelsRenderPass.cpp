#include "ModelsRenderPass.h"

#include "Utils/DrawData.h"
#include "RenderFlags.h"
#include "ResourceManagers/States/Samplers.h"

using namespace Renderer;
ModelsRenderPass::DrawCall::DrawCall(ModelsManager::SavedModel model, TexturesManager::TextureCache texture, const ModelDrawData& data) :
    model(model), texture(texture), data(data) {}

PipelineFactoryFlags ModelsRenderPass::ParseFlags(const ModelsFlags& flags)
{
    PipelineFactoryFlags flagsOut;
    flagsOut.definesFlags = 0;
    if (flags.isRed)
        flagsOut.definesFlags |= MRED;
    if (flags.isColored)
        flagsOut.definesFlags |= MCOLORED;

    flagsOut.pipelineFlags = flags.flags;

    return flagsOut;
}


ModelsRenderPass::ModelsRenderPass(BaseRenderSystem& renderSystem) : GachiBasePass({"ModelsShader.hlsl",renderSystem})
{
    uint32_t width, height;
    renderDevice->GetMainViewportSize(width, height);
    pDataCB = renderDevice->CreateConstBuffer(sizeof(dataBuffer));

    vp.x = 0;
    vp.y = 0;
    vp.w = width;
    vp.h = height;
    vp.minDepth = 0.0f;
    vp.maxDepth = 1.0f;
}

void ModelsRenderPass::Draw(const ModelDrawData& drawData)
{
    drawCalls.push_back(DrawCall(
            baseRendererParams.renderSystem.modelsManager->GetModel(drawData.modelId),
            baseRendererParams.renderSystem.texturesManger->GetImg(drawData.textureId),
            drawData)
    );
}



void ModelsRenderPass::Init(const char* dirr)
{
    BaseRenderPass::Init(dirr, new ModelsPassProvider());
}

void ModelsRenderPass::PreRender()
{
    RenderTargetBinding* targets[] = {
        baseRendererParams.renderSystem.texturesManger->GetRenderTarget(SID("preAAcolor")),
        baseRendererParams.renderSystem.texturesManger->GetRenderTarget(SID("lightColor")),
        baseRendererParams.renderSystem.texturesManger->GetRenderTarget(SID("velocityField")),
       // baseRendererParams.renderSystem.texturesManger->GetRenderTarget(SID("blurMask")),
        baseRendererParams.renderSystem.texturesManger->GetRenderTarget(SID("normalsField")),
        baseRendererParams.renderSystem.texturesManger->GetRenderTarget(SID("WorldPosition")),
    };

    renderDevice->SetRenderTargets(targets, std::size(targets), baseRendererParams.renderSystem.texturesManger->depthBuffer, vp);
    renderDevice->Clear(CLEAROPTIONS_TARGET, {}, 0, 0);
    renderDevice->Clear(CLEAROPTIONS_DEPTHBUFFER, {}, 1, 0);
}

void ModelsRenderPass::Resize()
{
    
    uint32_t width, height;
    renderDevice->GetMainViewportSize(width, height);
    
    vp.w = width;
    vp.h = height;
}

void ModelsRenderPass::Render()
{

    RenderTargetBinding* targets[] = {
        baseRendererParams.renderSystem.texturesManger->GetRenderTarget(SID("preAAcolor")),
        baseRendererParams.renderSystem.texturesManger->GetRenderTarget(SID("lightColor")),
        baseRendererParams.renderSystem.texturesManger->GetRenderTarget(SID("velocityField")),
        // baseRendererParams.renderSystem.texturesManger->GetRenderTarget(SID("blurMask")),
        baseRendererParams.renderSystem.texturesManger->GetRenderTarget(SID("normalsField")),
        baseRendererParams.renderSystem.texturesManger->GetRenderTarget(SID("WorldPosition")),
    };

    renderDevice->SetRenderTargets(targets, std::size(targets), baseRendererParams.renderSystem.texturesManger->depthBuffer, vp);


    size_t lastFlags = -1;

    renderDevice->VerifyConstBuffer(pDataCB, modelsCosntBuffer.slot);


    renderDevice->VerifyPixelSampler(0, Samplers::anisotropic16);


    for (size_t i = 0; i < drawCalls.size(); i++)
    {
        if (drawCalls[i].data.flags.flags != lastFlags)
        {
            PipelineFactoryFlags flags = ParseFlags(drawCalls[i].data.flags);
            renderDevice->ApplyPipelineState(factory->GetState(flags));
            lastFlags = drawCalls[i].data.flags.flags;
        }

        renderDevice->ApplyVertexBufferBinding(drawCalls[i].model.vertexBuffer);
        renderDevice->ApplyIndexBufferBinding(drawCalls[i].model.indexBuffer, drawCalls[i].model.indexBufferElementSize);


        dataBuffer.alpha = 1;
        dataBuffer.wh = float2(drawCalls[i].texture.width, drawCalls[i].texture.height);
        dataBuffer.color = {1,0,1};
        dataBuffer.world = drawCalls[i].data.newPosition.GetTransform();
        dataBuffer.oldWorld = drawCalls[i].data.oldPosition.GetTransform();
        dataBuffer.blurSwitch = 2.f;
        //if (drawCalls[i].data.isGun)
        //	dataBuffer.blurSwitch = 1.f;
        renderDevice->SetConstBuffer(pDataCB, &dataBuffer);
        renderDevice->VerifyPixelTexture(0, drawCalls[i].texture.texture);

        renderDevice->DrawIndexedPrimitives(
            drawCalls[i].model.pt, 0, 0, 0, 0,
            drawCalls[i].model.primitiveCount);
    }
}

void ModelsRenderPass::PostRender()
{
    drawCalls.clear();
}

ModelsRenderPass::~ModelsRenderPass()
{
    renderDevice->AddDisposeConstBuffer(pDataCB);
}

void ModelsRenderPass::SetupSettings(const RenderSettings& Settings)
{
    Init(Settings.shadersDirr);
}
