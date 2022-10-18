#pragma once
#include "IRenderer\RasterizerState.h"



struct RasterizerStates
{
    static Renderer::RasterizerState ClockWise;
    static Renderer::RasterizerState CClockWise;
    static Renderer::RasterizerState All;
    static Renderer::RasterizerState Wireframe;
    static int Init();
};