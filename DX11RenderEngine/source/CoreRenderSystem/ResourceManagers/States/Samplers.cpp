#include "pch.h"
#include "Samplers.h"

using namespace Renderer;

SamplerState Samplers::anisotropic16;
SamplerState Samplers::pointClamp;
SamplerState Samplers::linearClamp;
SamplerState Samplers::pointWrap;
SamplerState Samplers::linearWrap;

int Samplers::Init()
{

    
   anisotropic16.filter = TextureFilter::TEXTUREFILTER_ANISOTROPIC;
   anisotropic16.addressU = TextureAddressMode::TEXTUREADDRESSMODE_WRAP;
   anisotropic16.addressV = TextureAddressMode::TEXTUREADDRESSMODE_WRAP;
   anisotropic16.addressW = TextureAddressMode::TEXTUREADDRESSMODE_WRAP;
   anisotropic16.maxAnisotropy = 16;

    
    pointClamp.filter = TextureFilter::TEXTUREFILTER_POINT;
    pointClamp.addressU = TextureAddressMode::TEXTUREADDRESSMODE_CLAMP;
    pointClamp.addressV = TextureAddressMode::TEXTUREADDRESSMODE_CLAMP;
    pointClamp.addressW = TextureAddressMode::TEXTUREADDRESSMODE_CLAMP;

    
    linearClamp.filter = TextureFilter::TEXTUREFILTER_LINEAR;
    linearClamp.addressU = TextureAddressMode::TEXTUREADDRESSMODE_CLAMP;
    linearClamp.addressV = TextureAddressMode::TEXTUREADDRESSMODE_CLAMP;
    linearClamp.addressW = TextureAddressMode::TEXTUREADDRESSMODE_CLAMP;
    
    pointWrap.filter = TextureFilter::TEXTUREFILTER_POINT;
    pointWrap.addressU = TextureAddressMode::TEXTUREADDRESSMODE_WRAP;
    pointWrap.addressV = TextureAddressMode::TEXTUREADDRESSMODE_WRAP;
    pointWrap.addressW = TextureAddressMode::TEXTUREADDRESSMODE_WRAP;

    
    linearWrap.filter = TextureFilter::TEXTUREFILTER_LINEAR;
    linearWrap.addressU = TextureAddressMode::TEXTUREADDRESSMODE_WRAP;
    linearWrap.addressV = TextureAddressMode::TEXTUREADDRESSMODE_WRAP;
    linearWrap.addressW = TextureAddressMode::TEXTUREADDRESSMODE_WRAP;
    
    
    return 0;
}


int idbsdxfbsdzfxfbzdgbsdx = Samplers::Init();