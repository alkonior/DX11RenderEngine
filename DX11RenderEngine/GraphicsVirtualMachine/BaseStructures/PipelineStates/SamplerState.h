﻿#pragma once
#include <cmath>
#include "../../BaseStructures\GVMEnums.h"

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
        uint32_t               Data = 0;
        SamplerStateFields     Fields;
    };
    
    uint8_t                    BorderColor[4] = {0,0,0,0};
    float                      MipLODBias;
    float                      MinLOD;
    float                      MaxLOD;

    SamplerStateDesc (const GVM::SamplerStateDesc& );
};
    
}
struct SamplerStateDesc {
    using CompressedType = Compressed::SamplerStateDesc;
    
    ESamplerFilter             Filter = ESamplerFilter::FILTER_UNKNOWN;
    ETextureAddressMode        AddressU = ETextureAddressMode::TEXTURE_ADDRESS_UNKNOWN;
    ETextureAddressMode        AddressV = ETextureAddressMode::TEXTURE_ADDRESS_UNKNOWN;
    ETextureAddressMode        AddressW = ETextureAddressMode::TEXTURE_ADDRESS_UNKNOWN;
    float                      MipLODBias = 0;
    uint8_t                    MaxAnisotropy = 0;
    EComparisonFunc            ComparisonFunc = EComparisonFunc::COMPARISON_UNKNOWN;
    float                      BorderColor[4] = {};
    float                      MinLOD = 0;
    float                      MaxLOD = 0;

    SamplerStateDesc(const CompressedType& desc);
};

}