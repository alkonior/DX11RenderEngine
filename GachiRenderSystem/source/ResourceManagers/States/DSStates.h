#pragma once
#include "IRenderer\DepthStencilState.h"



struct DepthStencilStates
{
    static Renderer::DepthStencilState NoDSS;
    static Renderer::DepthStencilState DSS;
    static Renderer::DepthStencilState NoWrite;
    static int Init();
};