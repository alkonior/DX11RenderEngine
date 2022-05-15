#include "RasterizerState.h"
#include <cassert>
#include "BaseDefines.h"

using namespace GVM;

const RasterizerStateDesc RasterizerStateDesc::Default;

inline RasterizerStateDesc::RasterizerStateDesc() : BaseStateDesc(RasterizerStateMask) {}

RasterizerStateDesc::RasterizerStateDesc(uint64_t descriptor) : BaseStateDesc(0) {
    
    uint8_t shift = 0;

    State = GetBiteValue(descriptor, shift, 3);
    assert(State == RasterizerStateMask);
    
    FillMode = to_enum<EFillMode>(GetBiteValue(descriptor, shift, 1));
    CullMode = to_enum<ECullMode>(GetBiteValue(descriptor, shift, 2));
    
    FrontCounterClockwise  = GetBiteValue(descriptor, shift, 1);
    
    DepthBias = GetBiteValue(descriptor, shift, 32);
    DepthBiasClamp = GetBiteValue(descriptor, shift, 16);
    SlopeScaledDepthBias = GetBiteValue(descriptor, shift, 16);
    
    DepthClipEnable = GetBiteValue(descriptor, shift, 1);
    ScissorEnable = GetBiteValue(descriptor, shift, 1);
    MultisampleEnable = GetBiteValue(descriptor, shift, 1);
    AntialiasedLineEnable = GetBiteValue(descriptor, shift, 1);
    

}
uint64_t RasterizerStateDesc::ToUInt() {
    
    uint64_t result = State;
    
    result = (result<<1)|to_underlying(FillMode);
    result = (result<<2)|to_underlying(FillMode);
    
    result = (result<<1)|FrontCounterClockwise;
    result = (result<<32)|to_underlying(DepthBias);
    result = (result<<16)|to_underlying(DepthBiasClamp);
    result = (result<<16)|to_underlying(SlopeScaledDepthBias);
    
    result = (result<<1)|DepthClipEnable;
    result = (result<<1)|ScissorEnable;
    result = (result<<1)|MultisampleEnable;
    result = (result<<1)|AntialiasedLineEnable;

    return result;
    
}

