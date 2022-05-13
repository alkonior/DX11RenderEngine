#pragma once
#include "BaseStateDesc.h"

namespace GVM {

namespace Compressed {
    struct DepthStencilStateDesc {
        uint64_t data;
    };
}

struct DepthStencilDesc {
    EStencilOp      StencilFailOp;
    EStencilOp      StencilDepthFailOp;
    EStencilOp      StencilPassOp;
    EComparisonFunc StencilFunc;
}; 

struct DepthStencilStateDesc : BaseStateDesc
{
    using CompressedType = Compressed::DepthStencilStateDesc;
    inline static uint8_t DepthStencilStateMask = 3;
    
    bool                DepthEnable;
    bool                StencilEnable;
    EDepthWriteMask     DepthWriteMask;
    EComparisonFunc     DepthFunc;
    uint8_t             StencilReadMask;
    uint8_t             StencilWriteMask;
    DepthStencilDesc    FrontFace;
    DepthStencilDesc    BackFace;

    DepthStencilStateDesc();
    
    DepthStencilStateDesc(uint64_t descriptor);
    uint64_t ToUInt();
};

}


