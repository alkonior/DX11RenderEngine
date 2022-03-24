#include "BlendStates.h"
using namespace Renderer;

BlendState BlendStates::NoAlpha;
BlendState BlendStates::Alpha;

int BlendStates::Init()
{
    Alpha.enabled = true;
    Alpha.colorBlendFunction = BLENDFUNCTION_ADD;
    Alpha.alphaBlendFunction = BLENDFUNCTION_ADD;
    Alpha.colorSourceBlend = Blend::BLEND_SOURCEALPHA;
    Alpha.colorDestinationBlend = Blend::BLEND_INVERSESOURCEALPHA;
    Alpha.alphaSourceBlend = Blend::BLEND_ONE;
    Alpha.alphaDestinationBlend = Blend::BLEND_ZERO;
    Alpha.colorWriteEnable = ColorWriteChannels::COLORWRITECHANNELS_ALL ^ ColorWriteChannels::COLORWRITECHANNELS_ALPHA;
    Alpha.colorWriteEnable1 = ColorWriteChannels::COLORWRITECHANNELS_ALL;
    Alpha.colorWriteEnable2 = ColorWriteChannels::COLORWRITECHANNELS_ALL;
    Alpha.colorWriteEnable3 = ColorWriteChannels::COLORWRITECHANNELS_ALL;
    Alpha.multiSampleMask = -1;
    Alpha.blendFactor = Renderer::Color{ 255,255,255,255 };

    NoAlpha.enabled = false;
    NoAlpha.multiSampleMask = -1;
    NoAlpha.blendFactor = Renderer::Color{ 255,255,255,255 };

    NoAlpha.colorWriteEnable = ColorWriteChannels::COLORWRITECHANNELS_ALL;
    NoAlpha.colorWriteEnable1 = ColorWriteChannels::COLORWRITECHANNELS_ALL;
    NoAlpha.colorWriteEnable2 = ColorWriteChannels::COLORWRITECHANNELS_ALL;
    NoAlpha.colorWriteEnable3 = ColorWriteChannels::COLORWRITECHANNELS_ALL;
    
    return 0;
}


int idknvnvknclnsdfljvn = BlendStates::Init();