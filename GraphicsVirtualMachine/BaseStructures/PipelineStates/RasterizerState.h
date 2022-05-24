#pragma once
#include <cstdint>
#include "BaseStateDesc.h"
#include "BaseStructures/GVMEnums.h"


namespace GVM {
struct RasterizerStateDesc;
    
namespace Compressed {
    struct RasterizerStateDesc {
        
        RasterizerStateDesc(uint64_t data);
        RasterizerStateDesc(GVM::RasterizerStateDesc desc);
        
        RasterizerStateDesc(const RasterizerStateDesc& desc);
        RasterizerStateDesc(RasterizerStateDesc&& desc) noexcept;
        RasterizerStateDesc& operator=(const RasterizerStateDesc& desc);
        RasterizerStateDesc& operator=(RasterizerStateDesc&& desc) noexcept;
        ~RasterizerStateDesc() = default;

        struct RasterizerStateFields
        {
            
            EFillMode       FillMode = EFillMode::FILL_SOLID;
            ECullMode       CullMode = ECullMode::CULL_BACK;
            bool            FrontCounterClockwise = false;
            bool            DepthClipEnable = true;
            bool            ScissorEnable = false;
            bool            MultisampleEnable = false;
            bool            AntialiasedLineEnable = false;
            
        };
        
        union
        {
            uint64_t data;
            RasterizerStateFields Fields;
        };
    };
}

struct RasterizerStateDesc : BaseStateDesc
{
    using CompressedType = RasterizerStateDesc;

    static const RasterizerStateDesc Default;
    
    inline static uint8_t RasterizerStateMask = 2;
    
    EFillMode       FillMode = EFillMode::FILL_SOLID;
    ECullMode       CullMode = ECullMode::CULL_BACK;
    bool            FrontCounterClockwise = false;
    bool            DepthClipEnable = true;
    bool            ScissorEnable = false;
    bool            MultisampleEnable = false;
    bool            AntialiasedLineEnable = false;

    RasterizerStateDesc();
    
    RasterizerStateDesc(uint64_t descriptor);//todo
    CompressedType Compress() const;
};

}


