#pragma once
#include "IRenderer\BlendState.h"



struct BlendStates
{
    static Renderer::BlendState NoAlpha;
    static Renderer::BlendState Alpha;
    static Renderer::BlendState Add;
    static int Init();
};