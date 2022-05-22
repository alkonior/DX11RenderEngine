#pragma once
#include <cstdint>
#include "BaseStateDesc.h"
#include "BaseStructures/GVMEnums.h"


namespace GVM {

namespace Compressed {
    struct RasterizerStateDesc {
        uint64_t data;
    };
}

struct RasterizerStateDesc : BaseStateDesc
{
    using CompressedType = Compressed::RasterizerStateDesc;

    static const RasterizerStateDesc Default;
    
    inline static uint8_t RasterizerStateMask = 2;
    
    EFillMode       FillMode = EFillMode::FILL_SOLID;
    ECullMode       CullMode = ECullMode::CULL_BACK;
    bool            FrontCounterClockwise = false;
    int32_t         DepthBias = 0;
    int16_t         DepthBiasClamp = 0;
    int16_t         SlopeScaledDepthBias = 0.0;
    bool            DepthClipEnable = true;
    bool            ScissorEnable = false;
    bool            MultisampleEnable = false;
    bool            AntialiasedLineEnable = false;

    RasterizerStateDesc();
    
    RasterizerStateDesc(uint64_t descriptor);//todo
    uint64_t ToUInt() const;
};

}


