#include "DepthStencilState.h"
#include <cassert>
#include "BaseDefines.h"

using namespace GVM;

DepthStencilStateDesc::DepthStencilStateDesc() : BaseStateDesc(DepthStencilStateMask) {}

DepthStencilStateDesc::DepthStencilStateDesc(uint64_t descriptor) : BaseStateDesc(0) 
{
    uint8_t shift = 0;

    State = GetBiteValue(descriptor, shift, 3);
    assert(State == DepthStencilStateMask);
    
    DepthEnable = GetBiteValue(descriptor, shift, 1);
    StencilEnable = GetBiteValue(descriptor, shift, 1);
    
    DepthWriteMask = to_enum<EDepthWriteMask>(GetBiteValue(descriptor, shift, 1));
    
    DepthFunc = to_enum<EComparisonFunc>(GetBiteValue(descriptor, shift, 2));
    
    StencilReadMask = GetBiteValue(descriptor, shift, 8);
    StencilWriteMask = GetBiteValue(descriptor, shift, 8);


    FrontFace.StencilFunc        = to_enum<EComparisonFunc>(GetBiteValue(descriptor, shift, 2));
    FrontFace.StencilFailOp      = to_enum<EStencilOp>(GetBiteValue(descriptor, shift, 2));
    FrontFace.StencilPassOp      = to_enum<EStencilOp>(GetBiteValue(descriptor, shift, 2));
    FrontFace.StencilDepthFailOp = to_enum<EStencilOp>(GetBiteValue(descriptor, shift, 2));
    
    BackFace.StencilFunc        = to_enum<EComparisonFunc>(GetBiteValue(descriptor, shift, 2));
    BackFace.StencilFailOp      = to_enum<EStencilOp>(GetBiteValue(descriptor, shift, 2));
    BackFace.StencilPassOp      = to_enum<EStencilOp>(GetBiteValue(descriptor, shift, 2));
    BackFace.StencilDepthFailOp = to_enum<EStencilOp>(GetBiteValue(descriptor, shift, 2));
    
}

uint64_t DepthStencilStateDesc::ToUInt()
{
    
    uint64_t result = State;
    result = (result<<1)|DepthEnable;
    result = (result<<1)|StencilEnable;
    
    result = (result<<1)|to_underlying(DepthWriteMask);
    
    result = (result<<2)|to_underlying(DepthFunc);
    
    result = (result<<8)|StencilReadMask;
    result = (result<<8)|StencilWriteMask;
    
    result = (result<<2)|to_underlying(FrontFace.StencilFunc);
    result = (result<<2)|to_underlying(FrontFace.StencilFailOp);
    result = (result<<2)|to_underlying(FrontFace.StencilPassOp);
    result = (result<<2)|to_underlying(FrontFace.StencilDepthFailOp);

    result = (result<<2)|to_underlying(BackFace.StencilFunc);
    result = (result<<2)|to_underlying(BackFace.StencilFailOp);
    result = (result<<2)|to_underlying(BackFace.StencilPassOp);
    result = (result<<2)|to_underlying(BackFace.StencilDepthFailOp);
    

    return result;
    
}
