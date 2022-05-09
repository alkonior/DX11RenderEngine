#pragma once
#include "BaseStructures\GVMEnums.h"

namespace GVM {

struct SamplerStateDesc {
    
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
};

}