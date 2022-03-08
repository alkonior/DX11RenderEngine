#include "ImGUIRenderer.h"
#include "SimpleMath.h"
#include "Geometry.h"

void ImGUIRenderer::Init() {


}

void ImGUIRenderer::BeginFrame(GraphicsBase& gfx) {
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	

}

void ImGUIRenderer::Render() {
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void ImGUIRenderer::Destroy() {
}
