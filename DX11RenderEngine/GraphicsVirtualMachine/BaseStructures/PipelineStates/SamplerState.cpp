#include "SamplerState.h"

GVM::Compressed::SamplerStateDesc GVM::SamplerStateDesc::Compress() const {
    
    CompressedType result;

    result.MaxLOD = MaxLOD;
    result.MinLOD = MinLOD;
    result.MipLODBias = MipLODBias;


    result.BorderColor[0] = static_cast<uint8_t>(std::lroundf(255.f * BorderColor[0]));
    result.BorderColor[1] = static_cast<uint8_t>(std::lroundf(255.f * BorderColor[1]));
    result.BorderColor[2] = static_cast<uint8_t>(std::lroundf(255.f * BorderColor[2]));
    result.BorderColor[3] = static_cast<uint8_t>(std::lroundf(255.f * BorderColor[3]));
    
    //result.ColorArray =  r  | (g << 8) | (b << 16) | (b << 24);

    result.Fields.Filter = to_underlying(Filter);
    result.Fields.AddressU = to_underlying(AddressU);
    result.Fields.AddressV = to_underlying(AddressV);
    result.Fields.AddressW = to_underlying(AddressW);
    result.Fields.ComparisonFunc = to_underlying(ComparisonFunc);
    
     result.Fields.MaxAnisotropy = MaxAnisotropy;
    return result;
}
