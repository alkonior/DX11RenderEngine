#include "DepthStencilState.h"
#include <cassert>
#include "../../BaseDefines.h"

using namespace GVM;

const DepthStencilStateDesc DepthStencilStateDesc::Default;

Compressed::DepthStencilStateDesc::DepthStencilStateDesc(GVM::DepthStencilStateDesc desc):
data(desc.Compress().data)
{}

DepthStencilStateDesc::DepthStencilStateDesc() : BaseStateDesc(DepthStencilStateMask) {}

DepthStencilStateDesc::DepthStencilStateDesc(CompressedType descriptor) : BaseStateDesc(0) 
{
    //uint8_t shift = 0;

    State = descriptor.Fields.State;
    assert(State == DepthStencilStateMask);
    
    DepthEnable                  =  descriptor.Fields.DepthEnable                                    ;
    StencilEnable                =  descriptor.Fields.StencilEnable                                  ;
    DepthWriteMask               =  to_enum<EDepthWriteMask>(descriptor.Fields.DepthWriteMask)       ;
    DepthFunc                    =  to_enum<EComparisonFunc>(descriptor.Fields.DepthFunc)            ;
    StencilReadMask              =  descriptor.Fields.StencilReadMask                                ;
    StencilWriteMask             =  descriptor.Fields.StencilWriteMask                               ;

    FrontFace.StencilFunc        =  to_enum<EComparisonFunc>(descriptor.Fields.FrontStencilFunc)     ;
    FrontFace.StencilFailOp      =  to_enum<EStencilOp>(descriptor.Fields.FrontStencilFailOp)        ;
    FrontFace.StencilPassOp      =  to_enum<EStencilOp>(descriptor.Fields.FrontStencilPassOp)        ;
    FrontFace.StencilDepthFailOp =  to_enum<EStencilOp>(descriptor.Fields.FrontStencilDepthFailOp)   ;

    BackFace.StencilFunc         =  to_enum<EComparisonFunc>(descriptor.Fields.BackStencilFunc)      ;
    BackFace.StencilFailOp       =  to_enum<EStencilOp>(descriptor.Fields.BackStencilFailOp)         ;
    BackFace.StencilPassOp       =  to_enum<EStencilOp>(descriptor.Fields.BackStencilPassOp)         ;
    BackFace.StencilDepthFailOp  =  to_enum<EStencilOp>(descriptor.Fields.BackStencilDepthFailOp)    ;
    
}
Compressed::DepthStencilStateDesc DepthStencilStateDesc::Compress() const
{
    Compressed::DepthStencilStateDesc result;
    result.Fields.State                    = State;
    result.Fields.DepthEnable              = DepthEnable;
    result.Fields.StencilEnable            = StencilEnable;
    result.Fields.DepthWriteMask           = to_underlying(DepthWriteMask);
    result.Fields.DepthFunc                = to_underlying(DepthFunc);
    result.Fields.StencilReadMask          = StencilReadMask;
    result.Fields.StencilWriteMask         = StencilWriteMask;
    
    result.Fields.FrontStencilFunc         = to_underlying(FrontFace.StencilFunc);
    result.Fields.FrontStencilFailOp       = to_underlying(FrontFace.StencilFailOp);
    result.Fields.FrontStencilPassOp       = to_underlying(FrontFace.StencilPassOp);
    result.Fields.FrontStencilDepthFailOp  = to_underlying(FrontFace.StencilDepthFailOp);
    
    result.Fields.BackStencilFunc          = to_underlying(BackFace.StencilFunc);
    result.Fields.BackStencilFailOp        = to_underlying(BackFace.StencilFailOp);
    result.Fields.BackStencilPassOp        = to_underlying(BackFace.StencilPassOp);
    result.Fields.BackStencilDepthFailOp   = to_underlying(BackFace.StencilDepthFailOp);
    
    return result;
}

/*
uint64_t DepthStencilStateDesc::ToUInt() const
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
*/
