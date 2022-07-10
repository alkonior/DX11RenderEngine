#pragma once
#include "BaseStateDesc.h"
#include "../../BaseStructures/GVMEnums.h"

namespace GVM {

struct DepthStencilStateDesc;
    
namespace Compressed {
    struct DepthStencilStateDesc {
        DepthStencilStateDesc() = default;
        DepthStencilStateDesc(GVM::DepthStencilStateDesc desc);
        
        struct  DepthStencilStateFields{
            uint8_t State                      : 3;
            uint8_t DepthEnable                : 1;
            uint8_t StencilEnable              : 1;
            uint8_t DepthWriteMask             : 1;
            uint8_t DepthFunc                  : 4;
            uint8_t StencilReadMask            : 8;
            uint8_t StencilWriteMask           : 8;
            uint8_t FrontStencilFunc           : 2;
            uint8_t FrontStencilFailOp         : 2;
            uint8_t FrontStencilPassOp         : 2;
            uint8_t FrontStencilDepthFailOp    : 2;
            uint8_t BackStencilFunc            : 2;
            uint8_t BackStencilFailOp          : 2;
            uint8_t BackStencilPassOp          : 2;
            uint8_t BackStencilDepthFailOp     : 2;
        };
        
        union {
            uint64_t data = 0;
            DepthStencilStateFields Fields;
        };
    };
}

struct DepthStencilDesc {
    EStencilOp      StencilFailOp = EStencilOp::STENCIL_OP_KEEP;
    EStencilOp      StencilDepthFailOp = EStencilOp::STENCIL_OP_KEEP;
    EStencilOp      StencilPassOp = EStencilOp::STENCIL_OP_KEEP;
    EComparisonFunc StencilFunc = EComparisonFunc::COMPARISON_ALWAYS;
}; 

struct DepthStencilStateDesc : BaseStateDesc
{
    using CompressedType = Compressed::DepthStencilStateDesc;
    inline static uint8_t DepthStencilStateMask = 3;

    static const DepthStencilStateDesc Default;
    
    bool                DepthEnable = true;
    bool                StencilEnable = false;
    EDepthWriteMask     DepthWriteMask = EDepthWriteMask::DEPTH_WRITE_MASK_ALL;
    EComparisonFunc     DepthFunc = EComparisonFunc::COMPARISON_LESS;
    uint8_t             StencilReadMask = 0xff;
    uint8_t             StencilWriteMask = 0xff;
    DepthStencilDesc    FrontFace;
    DepthStencilDesc    BackFace;

    DepthStencilStateDesc();
    
    DepthStencilStateDesc(CompressedType descriptor);
    CompressedType Compress() const;
};

}


