#pragma once
#include "CoreRenderSystem/GraphicsBase.h"



class ImGUIRenderer {
public:
	void Init();

	void BeginFrame(GraphicsBase& gfx);
	void Render();
};