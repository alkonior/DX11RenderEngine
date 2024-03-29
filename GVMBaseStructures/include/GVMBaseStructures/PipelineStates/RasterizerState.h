#pragma once
#include <cstdint>
#include "GVMBaseStructures/PipelineStates/BaseStateDesc.h"
#include "GVMBaseStructures/GVMEnums.h"


namespace GVM {
struct RasterizerStateDesc;
    
namespace Compressed {
    struct RasterizerStateDesc {
        
        RasterizerStateDesc();
        RasterizerStateDesc(uint64_t data);
        RasterizerStateDesc(GVM::RasterizerStateDesc desc);
        
        RasterizerStateDesc(const RasterizerStateDesc& desc);
        RasterizerStateDesc(RasterizerStateDesc&& desc) noexcept;
        RasterizerStateDesc& operator=(const RasterizerStateDesc& desc);
        RasterizerStateDesc& operator=(RasterizerStateDesc&& desc) noexcept;
        ~RasterizerStateDesc() = default;

        struct RasterizerStateFields
        {
            uint8_t       State        : 3;
            uint8_t       FillMode              : 2;
            uint8_t       CullMode              : 2;
            uint8_t       FrontCounterClockwise : 1;
            uint8_t       DepthClipEnable       : 1;
            uint8_t       ScissorEnable         : 1;
            //uint8_t       MultisampleEnable     : 1;
            uint8_t       AntialiasedLineEnable : 1;
            
        };
        
        union
        {
            uint64_t data = 0;
            RasterizerStateFields Fields;
        };
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
    bool            DepthClipEnable = true;
    bool            ScissorEnable = false;
    //bool            MultisampleEnable = false;
    //bool            AntialiasedLineEnable = false;

    RasterizerStateDesc();
    
    RasterizerStateDesc(uint64_t descriptor);
    RasterizerStateDesc(CompressedType descriptor);
    CompressedType Compress() const;
};

}


