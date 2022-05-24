#pragma once
#include <cmath>
#include "BaseStructures\GVMEnums.h"

namespace GVM {

struct SamplerStateDesc;

namespace Compressed {
    
struct SamplerStateDesc {
    struct SamplerStateFields
    {
        uint8_t        Filter          : 6;
        uint8_t        AddressV        : 3;
        uint8_t        AddressW        : 3;
        uint8_t        MaxAnisotropy   ;
        uint8_t        ComparisonFunc  : 4;
        uint8_t        AddressU        : 3;
    };

    union
    {
        uint32_t               Data;
        SamplerStateFields     Fields;
    };
    
    uint8_t                    BorderColor[4];
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
    float                      BorderColor[4];
    float                      MinLOD;
    float                      MaxLOD;
    
    Compressed::SamplerStateDesc Compress() const;
};

}