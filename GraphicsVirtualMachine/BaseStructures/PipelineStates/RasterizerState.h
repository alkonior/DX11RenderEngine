#pragma once
#include <cstdint>

#include "BaseStateDesc.h"

namespace GVM {


struct RasterizerStateDesc : BaseStateDesc
{
    inline static uint8_t RasterizerStateMask = 2;
    
    EFillMode       FillMode;
    ECullMode       CullMode;
    bool            FrontCounterClockwise;
    int32_t         DepthBias;
    float           DepthBiasClamp;
    float           SlopeScaledDepthBias;
    bool            DepthClipEnable;
    bool            ScissorEnable;
    bool            MultisampleEnable;
    bool            AntialiasedLineEnable;

    RasterizerStateDesc();
    
    //RasterizerState(uint64_t descriptor);
    //uint64_t ToUInt();
};

}


