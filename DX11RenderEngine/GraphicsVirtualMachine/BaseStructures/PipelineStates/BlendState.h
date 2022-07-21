#pragma once
#include <cstdint>
#include "BaseStateDesc.h"
#include "../../BaseStructures\GVMEnums.h"

namespace GVM
{
struct  BlendStateDesc;

namespace Compressed {
    struct BlendStateDesc {
        
        BlendStateDesc();
        BlendStateDesc(uint64_t data);
        BlendStateDesc(GVM::BlendStateDesc desc);
        
        BlendStateDesc(const BlendStateDesc& desc);
        BlendStateDesc(BlendStateDesc&& desc) noexcept;
        BlendStateDesc& operator=(const BlendStateDesc& desc);
        BlendStateDesc& operator=(BlendStateDesc&& desc) noexcept;
        ~BlendStateDesc() = default;
        
        struct BlendStateFields
        {
            uint8_t   State        : 2;
            uint8_t   BlendEnable           : 1;
            uint8_t   LogicOpEnable         : 1;
            uint8_t   SrcBlend              : 5;
            uint8_t   DestBlend             : 5;
            uint8_t   SrcBlendAlpha         : 5;
            uint8_t   DestBlendAlpha        : 5;
            uint8_t   BlendOp               : 2;
            uint8_t   BlendOpAlpha          : 2;
            uint8_t   LogicOp               : 2;
            uint8_t   RenderTargetWriteMask : 2;
        };
        
        union 
        {
            uint64_t data = 0;
            BlendStateFields Fields;
        };

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
    ELogicOperator   LogicOp               = ELogicOperator::LOGIC_OP_UNKNOWN;
    uint8_t          RenderTargetWriteMask = 0xff;

    BlendStateDesc();
    BlendStateDesc(uint64_t descriptor);
    BlendStateDesc(CompressedType descriptor);
    
    CompressedType Compress() const;
};

struct CoreBlendDesc
{
    bool                IsBlendEnable           = false;
    uint32_t            SampleMask              = 0xffffffff;
    float               BlendFactor[4]          = {1,1,1,1};
};

namespace Compressed {

    struct CoreBlendDesc
    {
        bool                IsBlendEnable        = false;
        uint32_t            SampleMask           = 0xffffffff;
        float               BlendFactor[4]       = {1,1,1,1};
        BlendStateDesc      BlendStates[8]       = {};
    };

}
}
