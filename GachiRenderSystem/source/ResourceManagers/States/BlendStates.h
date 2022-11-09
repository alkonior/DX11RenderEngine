#pragma once
#include "IRenderer\BlendState.h"



struct BlendStates
{
    static Renderer::BlendState NoAlpha;
    static Renderer::BlendState Alpha;
    static Renderer::BlendState Add;
    static Renderer::BlendState Light;
    static int Init();
};