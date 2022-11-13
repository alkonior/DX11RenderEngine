#include "pch.h"


#include "RenderSystem.h"

#include "D3D11Renderer.h"

#include <filesystem>
#include <resource.h>
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"

#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"D3DCompiler.lib")


using namespace Renderer;

RenderSystem::RenderSystem(RenderEngineInitStruct init, const BaseRenderSystemInitStruct& bInit,
    ModelsManager* modelsManager,
    TexturesManager* texturesManager) :
    BaseRenderSystem(bInit),
    renderPassUI(*this),
    renderPassModels(*this),
    renderPassIMGUI({"",*this}),
    modelsManager(modelsManager),
    texturesManager(texturesManager),
    renderPassTAA(*this)
{
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange | ImGuiConfigFlags_ViewportsEnable;

    ImGui_ImplWin32_Init(init.hWnd1);
    ImGui_ImplDX11_Init(((D3D11Renderer*)pRenderer)->device.Get(), ((D3D11Renderer*)pRenderer)->context.Get());

    renderPasses.push_back(&renderPassUI);
    renderPasses.push_back(&renderPassIMGUI);
    renderPasses.push_back(&renderPassModels);
    renderPasses.push_back(&renderPassTAA);
    //managerImGUI.Init();
    //ImGui_ImplDX11_Init(pRenderer.device.Get(), pRenderer.context.Get());7

    //pLocalConstants = pRenderer->CreateConstBuffer(sizeof(taaConstants));
    //taaConstants.taaStrength = 1;
    //pRenderer->VerifyConstBuffer(pLocalConstants, taaShiftBuffer.slot);
////
//	//renderPasses.push_back(&managerSkybox);
//	//renderPasses.push_back(&managerUI);
//	//renderPasses.push_back(&managerModels);
//	//renderPasses.push_back(&managerMB);
//	//renderPasses.push_back(&managerUP);
//	//renderPasses.push_back(&managerPostProcess);
//	//renderPasses.push_back(&managerParticles);
//	//renderPasses.push_back(&managerBloom);
//	//renderPasses.push_back(&managerFXAA);
//	//renderPasses.push_back(&managerTAA);
//	//renderPasses.push_back(&managerSSAO);
//
}

void RenderSystem::ReinitShaders(const char* dirr)
{
    for (auto pass : renderPasses)
    {
        pass->Init(dirr);
    }
};

RenderSystem* RenderSystem::Initialise(RenderEngineInitStruct init)
{
    auto renderDevice = new Renderer::D3D11Renderer{
        {
            (int32_t)init.width,
            (int32_t)init.height,
            0,
            init.hWnd1,
            init.hWnd2,
            false,
            DepthFormat::DEPTHFORMAT_D32,
            PresentInterval::PRESENTINTERVAL_DEFAULT
        },
        1
    };

    auto t = new TexturesManager(renderDevice);
    auto m = new ModelsManager(renderDevice);
    return new RenderSystem{
        init,
        {
            renderDevice,t,m
        },
        m,t
    };
    //todo
}

void RenderSystem::BeginFrame()
{
    pRenderer->ClearState();
    for (auto pass : renderPasses)
    {
        pass->PreRender();
    }
}

#define GFX_CATCH_RENDER(render) try {render;} catch (const std::exception& exe) {printf_s(exe.what()); printf_s("\n"); static char c[100]; scanf_s("%s", c,1); success = false; }
//#define GFX_CATCH_RENDER(render) {render}

bool RenderSystem::RenderFrame()
{
    pRenderer->ClearState();

    //managerTAA.UpdateHaltonSequence();
    //taaConstants.taaPixelShift = managerTAA.HaltonSequence[managerTAA.HaltonIndex];
    //pRenderer->SetConstBuffer(pLocalConstants, &taaConstants);
    //pRenderer->VerifyConstBuffer(pLocalConstants, taaShiftBuffer.slot);

    BaseRenderSystem::Present();

    bool success = true;
    pRenderer->BeginEvent("BSP draw.");
    //GFX_CATCH_RENDER(managerUP.Render(*this););
    pRenderer->EndEvent();

    pRenderer->BeginEvent("BloomMask draw.");
    //GFX_CATCH_RENDER(managerBloom.RenderBloomMask(*this););
    pRenderer->EndEvent();

    pRenderer->BeginEvent("Static motion blur draw.");
    //GFX_CATCH_RENDER(managerMB.RenderStatic(*this););
    pRenderer->EndEvent();

    pRenderer->BeginEvent("Models draw.");
    try { renderPassModels.Render(); }
    catch (const std::exception& exe)
    {
        printf_s(exe.what());
        printf_s("\n");
        static char c[100];
        scanf_s("%s", c, 1);
        success = false;
    };
    pRenderer->EndEvent();

    pRenderer->BeginEvent("Dynamic motion blur draw.");
    //GFX_CATCH_RENDER(managerMB.RenderDynamic(*this););
    pRenderer->EndEvent();


    pRenderer->BeginEvent("SSAO draw.");
    //GFX_CATCH_RENDER(managerSSAO.Render(*this););
    pRenderer->EndEvent();

    pRenderer->BeginEvent("Sky draw.");
    //GFX_CATCH_RENDER(managerSkybox.Render(*this););
    pRenderer->EndEvent();

    pRenderer->BeginEvent("Particles draw.");
    //GFX_CATCH_RENDER(managerParticles.Render(*this););
    pRenderer->EndEvent();

    pRenderer->BeginEvent("Bloom pass.");
    //GFX_CATCH_RENDER(managerBloom.Render(*this););
    pRenderer->EndEvent();

    pRenderer->BeginEvent("End BSP draw.");
    //GFX_CATCH_RENDER(managerPostProcess.Render(*this););
    pRenderer->EndEvent();

    pRenderer->BeginEvent("TAA-pass.");
    try { renderPassTAA.Render();; }
    catch (const std::exception& exe)
    {
        printf_s(exe.what());
        printf_s("\n");
        static char c[100];
        scanf_s("%s", c, 1);
        success = false;
    };
    pRenderer->EndEvent();

    pRenderer->BeginEvent("UI draw.");
    GFX_CATCH_RENDER(renderPassUI.Render(););
    pRenderer->EndEvent();

    pRenderer->BeginEvent("IMGUI draw.");
    renderPassIMGUI.Render();
    pRenderer->EndEvent();


    return success;
}

void RenderSystem::EndFrame()
{
    PostRender();

    pRenderer->SwapBuffers();
}

void RenderSystem::ClearBuffer(sm::Vector4 color) noexcept
{
    pRenderer->SetRenderTargets(nullptr, 0, texturesManger->depthBuffer, Viewport());
    pRenderer->Clear((ClearOptions)7, {color.x,color.y,color.z,color.w}, 1, 0u);
    //pContext->ClearRenderTargetView(pTarget.Get(), reinterpret_cast<float*>(&color));
    //pContext->ClearDepthStencilView(pDSV.Get(), DModels11_CLEAR_DEPTH, 1.0f, 0u);
}

void RenderSystem::PostRender()
{
    //managerUP.Flush();

    for (auto pass : renderPasses)
    {
        pass->PostRender();
    }
}



void RenderSystem::DrawImg(size_t texId, const UIDrawData& data)
{
    //queue.push_back(DrawCall::draw2D).
    renderPassUI.Draw(texturesManger->GetImg(texId), data);
}


void RenderSystem::DrawColor(const UIDrawData& data)
{
    renderPassUI.Draw(data);
}

void RenderSystem::RegisterImg(size_t id, const TextureData& text)
{
    texturesManger->RegTexture(text, id);
}

void RenderSystem::UpdateImg(size_t id, const TextureData& text)
{
    texturesManger->UpdateTexture(text, id);
}

void RenderSystem::UpdateImg(const ImageUpdate& data)
{
    texturesManger->UpdateTexture(data);
}

void RenderSystem::ReleaseImg(size_t id)
{
    texturesManger->ReleaseTexture(id);
}

void RenderSystem::RegisterModel(size_t id, const ModelData& model)
{
    modelsManager->RegisterModel(id, model);
}

void RenderSystem::RegisterFramedModel(size_t id, const FramedModelData& model)
{
    modelsManager->RegisterModel(id, model);
}

void RenderSystem::ReleaseModel(size_t id)
{
    modelsManager->ReleaseModel(id);
}

void RenderSystem::RegisterImg(size_t id, int width, int height, void* data, bool mipmap)
{
    texturesManger->RegTexture(data, width, height, mipmap, id);
}

void RenderSystem::DrawModel(size_t modelId, size_t textureId, Transform position, size_t flags)
{
    renderPassModels.Draw(modelsManager->GetModel(modelId),
        texturesManger->GetImg(textureId),
        position, flags);
}

void RenderSystem::DrawUserPolygon(MeshHashData model, size_t textureId, UPDrawData data)
{
    //managerUP.Draw(model, texturesManger->GetImg(textureId), data);
}
void RenderSystem::DrawUserPolygon(MeshHashData model, size_t textureId, size_t lightmapId, UPDrawData data)
{
    //managerUP.Draw(model, texturesManger->GetImg(textureId), texturesManger->GetImg(lightmapId), data);
}

void RenderSystem::DrawSetUserPolygon(MeshHashData model, UPModelMesh newModel, size_t textureId, UPDrawData data)
{
    //managerUP.DrawSet(model, newModel, texturesManger->GetImg(textureId), data);
}

MeshHashData RenderSystem::RegisterhUserPolygon(UPModelMesh model, bool dynamic)
{
    return {};
    //return managerUP.Register(model, dynamic);
}

void RenderSystem::DrawFramedModel(size_t modelId, size_t textureId, const LerpModelDrawData& data)
{
    renderPassModels.DrawLerp(modelsManager->GetModel(modelId), texturesManger->GetImg(textureId), data);
}

void RenderSystem::DrawParticles(const ParticlesMesh& particles, const ParticlesDrawData& data)
{
    // managerParticles.Draw(particles, data);
}