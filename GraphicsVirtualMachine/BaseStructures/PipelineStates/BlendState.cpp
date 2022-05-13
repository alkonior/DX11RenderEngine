#include "BlendState.h"
#include <cassert>
#include "BaseDefines.h"

GVM::BlendStateDesc::BlendStateDesc() : BaseStateDesc(BlendStateMask) {}
GVM::BlendStateDesc::BlendStateDesc(uint64_t descriptor): BaseStateDesc(0)
{
    uint8_t shift = 0;

    State = GetBiteValue(descriptor, shift, 3);
    assert(State == BlendStateMask);
    
    BlendEnable = GetBiteValue(descriptor, shift, 1);
    LogicOpEnable = GetBiteValue(descriptor, shift, 1);
    
    SrcBlend       = to_enum<EBlendType>(GetBiteValue(descriptor, shift, 5));
    DestBlend      = to_enum<EBlendType>(GetBiteValue(descriptor, shift, 5));
    SrcBlendAlpha  = to_enum<EBlendType>(GetBiteValue(descriptor, shift, 5));
    DestBlendAlpha = to_enum<EBlendType>(GetBiteValue(descriptor, shift, 5));
    
    BlendOp        = to_enum<EBlendOperator>(GetBiteValue(descriptor, shift, 3));
    BlendOpAlpha   = to_enum<EBlendOperator>(GetBiteValue(descriptor, shift, 3));
    LogicOp        = to_enum<ELogicOperator>(GetBiteValue(descriptor, shift, 3));
    RenderTargetWriteMask  = GetBiteValue(descriptor, shift, 8);

}

uint64_t GVM::BlendStateDesc::ToUInt()
{
    uint64_t result = State;
    result = (result<<1)|BlendEnable;
    result = (result<<1)|LogicOpEnable;
    
    result = (result<<5)|to_underlying(SrcBlend);
    result = (result<<5)|to_underlying(DestBlend);
    result = (result<<5)|to_underlying(SrcBlendAlpha);
    result = (result<<5)|to_underlying(DestBlendAlpha);
    
    result = (result<<3)|to_underlying(BlendOp);
    result = (result<<3)|to_underlying(BlendOpAlpha);
    result = (result<<3)|to_underlying(LogicOp);
    result = (result<<8)|RenderTargetWriteMask;

    return result;
}
