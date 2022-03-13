#pragma once
#include "IRenderer\SamplerState.h"



struct Samplers
{
    static Renderer::SamplerState point;
    static Renderer::SamplerState anisotropic16;
    static int Init();
};