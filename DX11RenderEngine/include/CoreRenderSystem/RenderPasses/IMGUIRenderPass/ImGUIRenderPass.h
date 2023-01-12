#pragma once
#include "CoreRenderSystem/RenderPasses/BaseRenderPass.h"
//#include "imgui/imgui.h"
//#include "imgui/imgui_impl_win32.h"
//#include "imgui/imgui_impl_dx11.h"


class ImGUIRenderPass final : public BaseRenderPass {
public:
    explicit ImGUIRenderPass(const BaseRendererParams& in);
    void Init(const char* dirr) override;
    void Resize() override;
    void Render();
    void PreRender() override;
    void PostRender() override;
    ~ImGUIRenderPass() override;



};