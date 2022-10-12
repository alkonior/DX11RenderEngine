#pragma once
#include "IRenderer\SamplerState.h"



struct Samplers
{
    static Renderer::SamplerState pointClamp;
    static Renderer::SamplerState pointWrap;
    static Renderer::SamplerState linearClamp;
    static Renderer::SamplerState linearWrap;
    static Renderer::SamplerState anisotropic16;
    static int Init();
};