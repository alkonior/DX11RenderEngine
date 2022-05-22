#pragma once
#include <cstdint>
#include "BaseStateDesc.h"
#include "BaseStructures/GVMEnums.h"

namespace GVM
{

namespace Compressed {
    struct BlendStateDesc {
        uint64_t data;
    };
}

struct BlendStateDesc : BaseStateDesc
{
    using CompressedType = Compressed::BlendStateDesc;
    
    inline static uint8_t BlendStateMask   = 1;
    bool             BlendEnable           = false;
    bool             LogicOpEnable         = false;
    EBlendType       SrcBlend              = EBlendType::BLEND_ONE;
    EBlendType       DestBlend             = EBlendType::BLEND_ZERO; 
    EBlendType       SrcBlendAlpha         = EBlendType::BLEND_ONE;
    EBlendType       DestBlendAlpha        = EBlendType::BLEND_ZERO;
    EBlendOperator   BlendOp               = EBlendOperator::BLEND_OP_ADD;
    EBlendOperator   BlendOpAlpha          = EBlendOperator::BLEND_OP_ADD;
    ELogicOperator   LogicOp               = ELogicOperator::LOGIC_OP_UNDEFINED;
    uint8_t          RenderTargetWriteMask = 0xff;

    BlendStateDesc();
    BlendStateDesc(uint64_t descriptor);
    uint64_t ToUInt() const;
};

struct CoreBlendDesc
{
    bool                IsBlendEnable           = false;
    uint32_t            SampleMask              = 0xffffffff;
    float               BlendFactor[4]          = {1,1,1,1};
};
   
}
