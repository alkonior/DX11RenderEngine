#pragma once
#include "BaseStructures\GVMEnums.h"

namespace GVM {

struct SamplerStateDesc;

namespace Compressed {
    
struct SamplerStateDesc {

    //ESamplerFilter             Filter;
    //ETextureAddressMode        AddressU;
    //ETextureAddressMode        AddressV;
    //ETextureAddressMode        AddressW;
    //EComparisonFunc            ComparisonFunc;
    //uint8_t                    MaxAnisotropy;
    uint32_t                   Compressed;
    
    uint32_t                   ColorArray;
    float                      MipLODBias;
    float                      MinLOD;
    float                      MaxLOD;

    SamplerStateDesc Decompress();
};
    
}
struct SamplerStateDesc {
    using CompressedType = Compressed::SamplerStateDesc;
    
    ESamplerFilter             Filter;
    ETextureAddressMode        AddressU;
    ETextureAddressMode        AddressV;
    ETextureAddressMode        AddressW;
    float                      MipLODBias;
    uint8_t                    MaxAnisotropy;
    EComparisonFunc            ComparisonFunc;
    uint8_t                    BorderColor[4];
    float                      MinLOD;
    float                      MaxLOD;
    
    Compressed::SamplerStateDesc Compress();
};

}