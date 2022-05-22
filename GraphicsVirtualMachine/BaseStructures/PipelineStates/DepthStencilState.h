#pragma once
#include "BaseStateDesc.h"
#include "BaseStructures/GVMEnums.h"

namespace GVM {

namespace Compressed {
    struct DepthStencilStateDesc {
        uint64_t data;
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
    
    DepthStencilStateDesc(uint64_t descriptor);
    uint64_t ToUInt() const;
};

}


