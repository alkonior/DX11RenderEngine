#include "SamplerState.h"

GVM::Compressed::SamplerState GVM::SamplerStateDesc::Compress() {
    
    GVM::Compressed::SamplerState result;

    result.MaxLOD = MaxLOD;
    result.MinLOD = MinLOD;
    result.MipLODBias = MipLODBias;


    uint32_t r = 255 * BorderColor[0];
    uint32_t b = 255 * BorderColor[1];
    uint32_t g = 255 * BorderColor[2];
    uint32_t a = 255 * BorderColor[3];
    
    result.ColorArray =  r  | (g << 8) | (b << 16) | (b << 24);

    result.Compressed = (result.Compressed<<6)|to_underlying(Filter);
    result.Compressed = (result.Compressed<<3)|to_underlying(AddressU);
    result.Compressed = (result.Compressed<<3)|to_underlying(AddressV);
    result.Compressed = (result.Compressed<<3)|to_underlying(AddressW);
    result.Compressed = (result.Compressed<<3)|to_underlying(ComparisonFunc);
    
    result.Compressed = (result.Compressed<<8)|MaxAnisotropy;
}
