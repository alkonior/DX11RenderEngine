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
	static bool show = true;
	ImGui::ShowDemoWindow(&show);
	
	ImGuiIO& io = ImGui::GetIO();
	
	
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}

