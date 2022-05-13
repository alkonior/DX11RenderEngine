#pragma once
#include <cstdint>

#include "BaseStateDesc.h"

namespace GVM {

namespace Compressed {
    struct RasterizerStateDesc {
        uint64_t data;
    };
}

struct RasterizerStateDesc : BaseStateDesc
{
    using CompressedType = Compressed::RasterizerStateDesc;
    inline static uint8_t RasterizerStateMask = 2;
    
    EFillMode       FillMode;
    ECullMode       CullMode;
    bool            FrontCounterClockwise;
    int32_t         DepthBias;
    int16_t         DepthBiasClamp;
    int16_t         SlopeScaledDepthBias;
    bool            DepthClipEnable;
    bool            ScissorEnable;
    bool            MultisampleEnable;
    bool            AntialiasedLineEnable;

    RasterizerStateDesc();
    
    RasterizerStateDesc(uint64_t descriptor);//todo
    uint64_t ToUInt();
};

}


