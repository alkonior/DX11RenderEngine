#pragma once
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"


class ImGUIRenderer {
public:
	void Init();

	void Render();

	void Destroy();

};