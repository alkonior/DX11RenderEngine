#include "DepthStencilState.h"
#include <cassert>
#include "../../BaseDefines.h"

using namespace GVM;

const DepthStencilStateDesc DepthStencilStateDesc::Default;

Compressed::DepthStencilStateDesc::DepthStencilStateDesc():DepthStencilStateDesc(GVM::DepthStencilStateDesc()) {}
Compressed::DepthStencilStateDesc::DepthStencilStateDesc(const GVM::DepthStencilStateDesc& desc)
{
    Fields.State                    = desc.State;
    Fields.DepthEnable              = desc.DepthEnable;
    Fields.StencilEnable            = desc.StencilEnable;
    Fields.DepthWriteMask           = to_underlying(desc.DepthWriteMask);
    Fields.DepthFunc                = to_underlying(desc.DepthFunc);
    Fields.StencilReadMask          = desc.StencilReadMask;
    Fields.StencilWriteMask         = desc.StencilWriteMask;
    
    Fields.FrontStencilFunc         = to_underlying(desc.FrontFace.StencilFunc);
    Fields.FrontStencilFailOp       = to_underlying(desc.FrontFace.StencilFailOp);
    Fields.FrontStencilPassOp       = to_underlying(desc.FrontFace.StencilPassOp);
    Fields.FrontStencilDepthFailOp  = to_underlying(desc.FrontFace.StencilDepthFailOp);
    
    Fields.BackStencilFunc          = to_underlying(desc.BackFace.StencilFunc);
    Fields.BackStencilFailOp        = to_underlying(desc.BackFace.StencilFailOp);
    Fields.BackStencilPassOp        = to_underlying(desc.BackFace.StencilPassOp);
    Fields.BackStencilDepthFailOp   = to_underlying(desc.BackFace.StencilDepthFailOp);
    
}
Compressed::DepthStencilStateDesc::DepthStencilStateDesc(const DepthStencilStateDesc& desc)
:data(desc.data) {}
Compressed::DepthStencilStateDesc& Compressed::DepthStencilStateDesc::operator=(const DepthStencilStateDesc& desc)
{
    data=desc.data;
    return *this;
}
Compressed::DepthStencilStateDesc::DepthStencilStateDesc(const DepthStencilStateDesc&& desc) noexcept
:data(desc.data){}


DepthStencilStateDesc::DepthStencilStateDesc() : BaseStateDesc(DepthStencilStateMask) {}

DepthStencilStateDesc::DepthStencilStateDesc(const CompressedType& descriptor) : BaseStateDesc(0) 
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
