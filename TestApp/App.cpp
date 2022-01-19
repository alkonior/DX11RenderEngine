#pragma once
#include "App.h"

App::App()
	:
	wnd(600, 600, "The Donkey Fart Box") {
	wnd.RD.RegisterTexture(1, L"Img/kappa50.png");
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
	wnd.RD.DrawImg(1, 0, 0, 100, 100, 0);
	wnd.RD.DrawImg(1, 0, 0, 800, 800, 100, 100, 100, 100, 0);
	wnd.RD.DrawImg(1, 20, 20, 20, 20, 200, 200, 100, 100, 1);
	wnd.RD.Present();
}