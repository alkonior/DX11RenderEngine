#include "OpaqueRenderPass.h"

#include "RenderFlags.h"
#include "ResourceManagers/States/Samplers.h"

using namespace Renderer;
OpaqueRenderPass::DrawCall::DrawCall(ModelsManager::SavedModel model, TexturesManager::TextureCache texture, const ModelDrawData& data) :
    model(model), texture(texture), data(data) {}

PipelineFactoryFlags OpaqueRenderPass::ParseFlags(const ModelsFlags& flags)
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


OpaqueRenderPass::OpaqueRenderPass(BaseRenderSystem& renderSystem) : GachiBasePass({"ModelsShader.hlsl",renderSystem})
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

void OpaqueRenderPass::Draw(const ModelDrawData& drawData)
{
    drawCalls.push_back(DrawCall(
            baseRendererParams.renderSystem.modelsManager->GetModel(drawData.modelId),
            baseRendererParams.renderSystem.texturesManger->GetImg(drawData.textureId),
            drawData)
    );
}



void OpaqueRenderPass::Init(const char* dirr)
{
    BaseRenderPass::Init(dirr, new ModelsPassProvider());
}

void OpaqueRenderPass::PreRender()
{
    RenderTargetBinding* targets[5] = {
        baseRendererParams.renderSystem.texturesManger->GetRenderTarget(SID("preAAcolor")),
        baseRendererParams.renderSystem.texturesManger->GetRenderTarget(SID("lightColor")),
        baseRendererParams.renderSystem.texturesManger->GetRenderTarget(SID("velocityField")),
        baseRendererParams.renderSystem.texturesManger->GetRenderTarget(SID("blurMask")),
        baseRendererParams.renderSystem.texturesManger->GetRenderTarget(SID("normalsField")),
    };

    renderDevice->SetRenderTargets(targets, std::size(targets), baseRendererParams.renderSystem.texturesManger->depthBuffer, vp);
    renderDevice->Clear(CLEAROPTIONS_TARGET, {}, 0, 0);
    renderDevice->Clear(CLEAROPTIONS_DEPTHBUFFER, {}, 1, 0);
}

void OpaqueRenderPass::Resize()
{
    
    uint32_t width, height;
    renderDevice->GetMainViewportSize(width, height);
    
    vp.w = width;
    vp.h = height;
}

void OpaqueRenderPass::Render()
{

    RenderTargetBinding* targets[] = {
        baseRendererParams.renderSystem.texturesManger->GetRenderTarget(SID("preAAcolor")),
        baseRendererParams.renderSystem.texturesManger->GetRenderTarget(SID("lightColor")),
        baseRendererParams.renderSystem.texturesManger->GetRenderTarget(SID("velocityField")),
        // baseRendererParams.renderSystem.texturesManger->GetRenderTarget(SID("blurMask")),
        baseRendererParams.renderSystem.texturesManger->GetRenderTarget(SID("normalsField")),
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

void OpaqueRenderPass::PostRender()
{
    drawCalls.clear();
}

OpaqueRenderPass::~OpaqueRenderPass()
{
    renderDevice->AddDisposeConstBuffer(pDataCB);
}

void OpaqueRenderPass::SetupSettings(const RenderSettings& Settings)
{
    Init(Settings.shadersDirr);
}
