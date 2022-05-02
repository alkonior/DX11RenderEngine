#pragma once
#include <cstdint>
#include "BaseStateDesc.h"

namespace GVM
{

struct BlendStateDesc : BaseStateDesc
{
    inline static uint8_t BlendStateMask = 1;
    bool             BlendEnable = false;
    bool             LogicOpEnable = false;
    EBlendType       SrcBlend;
    EBlendType       DestBlend;
    EBlendType       SrcBlendAlpha;
    EBlendType       DestBlendAlpha;
    EBlendOperator   BlendOp;
    EBlendOperator   BlendOpAlpha;
    ELogicOperator   LogicOp;
    uint8_t          RenderTargetWriteMask;

    BlendStateDesc();
    BlendStateDesc(uint64_t descriptor);
    uint64_t ToUInt();
};

struct BlendDesc : BaseStateDesc
{
    bool                AlphaToCoverageEnable = false;
    bool                IndependentBlendEnable = false;
    BlendStateDesc      RenderTarget[8];
-
};
   
}
