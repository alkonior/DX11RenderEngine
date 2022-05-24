#include "BlendState.h"
#include <cassert>
#include "BaseDefines.h"

using namespace GVM;

Compressed::BlendStateDesc::BlendStateDesc(uint64_t data): data(data)
{
#ifdef _DEBUG
    assert(Fields.BlendStateMask == GVM::BlendStateDesc::BlendStateMask);
#endif
}

Compressed::BlendStateDesc::BlendStateDesc(const BlendStateDesc& desc): data(desc.data)
{}

Compressed::BlendStateDesc::BlendStateDesc(BlendStateDesc&& desc) noexcept : data(desc.data)
{}

Compressed::BlendStateDesc& Compressed::BlendStateDesc::operator=(const BlendStateDesc& desc)
{
    this->data = desc.data;
    return *this;
}

Compressed::BlendStateDesc& Compressed::BlendStateDesc::operator=(BlendStateDesc&& desc) noexcept
{
    this->data = desc.data;
    return *this;
}


Compressed::BlendStateDesc::BlendStateDesc(GVM::BlendStateDesc desc)
{
     Fields.BlendStateMask        = desc.State;//3
     Fields.BlendEnable           = desc.BlendEnable;
     Fields.LogicOpEnable         = desc.LogicOpEnable;
    
     Fields.SrcBlend              = to_underlying(desc.SrcBlend);
     Fields.DestBlend             = to_underlying(desc.DestBlend);
     Fields.SrcBlendAlpha         = to_underlying(desc.SrcBlendAlpha);
     Fields.DestBlendAlpha        = to_underlying(desc.DestBlendAlpha);

     Fields.BlendOp               = to_underlying(desc.BlendOp);
     Fields.BlendOpAlpha          = to_underlying(desc.BlendOpAlpha);
     Fields.LogicOp               = to_underlying(desc.LogicOp);
     Fields.RenderTargetWriteMask = desc.RenderTargetWriteMask;
}

BlendStateDesc::BlendStateDesc() : BaseStateDesc(BlendStateMask) {}
BlendStateDesc::BlendStateDesc(uint64_t descriptor): BaseStateDesc(0)
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
//
// uint64_t BlendStateDesc::ToUInt() const
// {
//     uint64_t result = State;//3
//     result = (result<<1)|BlendEnable;
//     result = (result<<1)|LogicOpEnable;
//     
//     result = (result<<5)|to_underlying(SrcBlend);
//     result = (result<<5)|to_underlying(DestBlend);
//     result = (result<<5)|to_underlying(SrcBlendAlpha);
//     result = (result<<5)|to_underlying(DestBlendAlpha);
//     
//     result = (result<<3)|to_underlying(BlendOp);
//     result = (result<<3)|to_underlying(BlendOpAlpha);
//     result = (result<<3)|to_underlying(LogicOp);
//     result = (result<<8)|RenderTargetWriteMask;
//
//     return result;
// }
