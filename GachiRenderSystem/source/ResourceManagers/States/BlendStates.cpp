#include "BlendStates.h"
using namespace Renderer;

BlendState BlendStates::NoAlpha;
BlendState BlendStates::Alpha;
BlendState BlendStates::Add;
BlendState BlendStates::Light;

int BlendStates::Init()
{
    Alpha.enabled = true;
    Alpha.colorBlendFunction = BLENDFUNCTION_ADD;
    Alpha.alphaBlendFunction = BLENDFUNCTION_ADD;
    Alpha.colorSourceBlend = Blend::BLEND_SOURCEALPHA;
    Alpha.colorDestinationBlend = Blend::BLEND_INVERSESOURCEALPHA;
    Alpha.alphaSourceBlend = Blend::BLEND_ONE;
    Alpha.alphaDestinationBlend = Blend::BLEND_ZERO;
    Alpha.colorWriteEnable  = ColorWriteChannels::COLORWRITECHANNELS_ALL;
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


    
    Add.enabled = true;
    Add.colorBlendFunction = BLENDFUNCTION_ADD;
    Add.alphaBlendFunction = BLENDFUNCTION_ADD;
    Add.colorSourceBlend = Blend::BLEND_ONE;
    Add.colorDestinationBlend = Blend::BLEND_ZERO;
    Add.alphaSourceBlend = Blend::BLEND_ONE;
    Add.alphaDestinationBlend = Blend::BLEND_ZERO;
    Add.colorWriteEnable  = ColorWriteChannels::COLORWRITECHANNELS_ALL;
    Add.colorWriteEnable1 = ColorWriteChannels::COLORWRITECHANNELS_ALL;
    Add.colorWriteEnable2 = ColorWriteChannels::COLORWRITECHANNELS_ALL;
    Add.colorWriteEnable3 = ColorWriteChannels::COLORWRITECHANNELS_ALL;
    Add.multiSampleMask = -1;
    Add.blendFactor = Renderer::Color{ 255,255,255,255 };
    
    Light.enabled = true;
    Light.colorBlendFunction = BLENDFUNCTION_ADD;
    Light.alphaBlendFunction = BLENDFUNCTION_ADD;
    Light.colorSourceBlend = Blend::BLEND_SOURCEALPHA;
    Light.colorDestinationBlend = Blend::BLEND_ONE;
    Light.alphaSourceBlend = Blend::BLEND_ONE;
    Light.alphaDestinationBlend = Blend::BLEND_ZERO;
    Light.colorWriteEnable  = ColorWriteChannels::COLORWRITECHANNELS_ALL;
    Light.colorWriteEnable1 = ColorWriteChannels::COLORWRITECHANNELS_ALL;
    Light.colorWriteEnable2 = ColorWriteChannels::COLORWRITECHANNELS_ALL;
    Light.colorWriteEnable3 = ColorWriteChannels::COLORWRITECHANNELS_ALL;
    Light.multiSampleMask = -1;
    Light.blendFactor = Renderer::Color{ 255,255,255,255 };
    
    return 0;
}


int idknvnvknclnsdfljvn = BlendStates::Init();