#include "Samplers.h"
using namespace Renderer;

SamplerState Samplers::anisotropic16;
SamplerState Samplers::point;

int Samplers::Init()
{
   point.filter = TextureFilter::TEXTUREFILTER_POINT;
   point.addressU = TextureAddressMode::TEXTUREADDRESSMODE_WRAP;
   point.addressV = TextureAddressMode::TEXTUREADDRESSMODE_WRAP;
   point.addressW = TextureAddressMode::TEXTUREADDRESSMODE_WRAP;

    
   anisotropic16.filter = TextureFilter::TEXTUREFILTER_ANISOTROPIC;
   anisotropic16.addressU = TextureAddressMode::TEXTUREADDRESSMODE_WRAP;
   anisotropic16.addressV = TextureAddressMode::TEXTUREADDRESSMODE_WRAP;
   anisotropic16.addressW = TextureAddressMode::TEXTUREADDRESSMODE_WRAP;
   anisotropic16.maxAnisotropy = 16;
    
    
    return 0;
}


int idbsdxfbsdzfxfbzdgbsdx = Samplers::Init();