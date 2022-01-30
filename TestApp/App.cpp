#pragma once
#include "App.h"
#include "Geometry.h"

App::App()
	:
	wnd(600, 600, "The Donkey Fart Box") {
	wnd.RD.RegisterTexture(1, L"Img/kappa50.png");
	{
		std::vector<DirectX::VertexPositionNormalTexture> vertices;
		std::vector<uint16_t> indices;

		ComputeBox(vertices, indices, { 1.0,1.0,1.0 }, false, false);


		std::vector<ModelVertex> myVerticies;
		for (size_t i = 0; i < vertices.size(); i++) {
			myVerticies.push_back(ModelVertex{ vertices[i].position, vertices[i].normal, vertices[i].textureCoordinate });
		}
		Renderer::PrimitiveType pt = Renderer::PrimitiveType::PRIMITIVETYPE_TRIANGLELIST;
		size_t primitiveCount = indices.size() / 3;

		ModelData cube = ModelData(myVerticies, indices, pt, primitiveCount);
		wnd.RD.RegisterModel(0, cube);
	}

	{
		std::vector<DirectX::VertexPositionNormalTexture> vertices;
		std::vector<uint16_t> indices;

		ComputeBox(vertices, indices, { 1.0,1.0,1.0 }, false, false);


		std::vector<FramedModelVertex> myVerticies;
		for (size_t i = 0; i < vertices.size(); i++) {
			myVerticies.push_back(FramedModelVertex{ vertices[i].normal, vertices[i].textureCoordinate });
		}
		std::vector<Frame> frames(2);
		for (size_t i = 0; i < vertices.size(); i++) {
			frames[0].push_back(vertices[i].position);
			frames[1].push_back(vertices[i].position + float3{ 1, 1 ,0 });
		}

		Renderer::PrimitiveType pt = Renderer::PrimitiveType::PRIMITIVETYPE_TRIANGLELIST;
		size_t primitiveCount = indices.size() / 3;

		FramedModelData framedCube = FramedModelData(myVerticies, frames, indices, pt, primitiveCount);

		wnd.RD.RegisterFramedModel(1, framedCube);
	}

}

int App::Go() {
	while (true) {
		// process all messages pending, but to not block for new messages
		if (const auto ecode = Window::ProcessMessages()) {
			// if return optional has value, means we're quitting so return exit code
			return *ecode;
		}
		DoFrame();
	}
}

void App::DoFrame() {
	const float c = sin(timer.Peek()) / 2.0f + 0.5f;
	wnd.RD.Clear(0.0f, 0.0f, 0.0f);
	wnd.RD.BeginFrame();

	wnd.RD.DrawImg(1, 0, 0, 100, 100, 0);
	wnd.RD.DrawImg(1, 0, 0, 800, 800, 100, 100, 100, 100, 0);
	//wnd.RD.DrawImg(1, 20, 20, 20, 20, 200, 200, 100, 100, 1);

	static bool show_alpha_window = true;

	ImGui::Begin("Alpha:", &show_alpha_window, 0);
	static float alpha[] = { 0.0f, 0.0f };
	ImGui::SliderFloat("alpha1", alpha, -1, 2);
	ImGui::SliderFloat("alpha2", alpha + 1, -1, 2);
	
	wnd.RD.DrawFramedModel(1, 1, Transform({ -100, 100, -200 }, { 0, 0, 0 }, { 100,100,100 }), 0, 1, alpha[0], 1 | 2);
	wnd.RD.DrawFramedModel(1, 1, Transform({    0, 100, -200 }, { 0, 0, 0 }, { 100,100,100 }), 1, 0, alpha[1], 0 | 2);
	
	ImGui::End();

	wnd.RD.DrawModel(0, 1, Transform({ -100, -100, -200 }, { 0.5, 0, 0.5 }, { 100,100,100 }), 1);
	wnd.RD.DrawModel(0, 1, Transform({ 0, -100, -200 }, { 0.5, 0, 0.5 }, { 100,100,100 }), 0);
	wnd.RD.Present();
}