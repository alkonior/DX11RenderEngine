#include "ImGUIRenderer.h"
#include "SimpleMath.h"
#include "Geometry.h"
#include "ModelData.h"

void ImGUIRenderer::Init() {

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

}

void ImGUIRenderer::BeginFrame(GraphicsBase& gfx) {
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	static bool show_demo_window = true;
	if (show_demo_window) {
		ImGui::ShowDemoWindow(&show_demo_window);
	}

	static bool show_camera_window = true;
	ImGui::Begin("Camera Controller", &show_camera_window, 0);

	static float position[] = { 0.0f, 0.0f, -100.0f };
	ImGui::Text("Camera Position:");
	ImGui::SliderFloat("x", position, -1000.0f, 1000.0f);
	ImGui::SliderFloat("y", position + 1, -1000.0f, 1000.0f);
	ImGui::SliderFloat("z", position + 2, -1000.0f, 1000.0f);

# define M_PI           3.14159265358979323846f  /* pi */

	static float angles[] = { 0.0f, 0.0f, 0.0f };
	ImGui::Text("Camera Angle:");
	ImGui::SliderFloat("row", angles, -M_PI, M_PI);
	ImGui::SliderFloat("pitch", angles + 1, -M_PI, M_PI);
	ImGui::SliderFloat("yaw", angles + 2, -M_PI, M_PI);


	//gfx.SetCameraPosition(Transform({ position[0],position[1],-position[2] }, { angles[0],angles[1],angles[2] }, { 1,1,1 }));

	ImGui::End();
}

void ImGUIRenderer::Render() {

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void ImGUIRenderer::Destroy() {
	ImGui::DestroyContext();
}
