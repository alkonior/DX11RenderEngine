#include "RasterizerState.h"
#include <cassert>
#include "../../BaseDefines.h"

using namespace GVM;

const RasterizerStateDesc RasterizerStateDesc::Default;

Compressed::RasterizerStateDesc::RasterizerStateDesc():RasterizerStateDesc(GVM::RasterizerStateDesc()) {}
Compressed::RasterizerStateDesc::RasterizerStateDesc(uint64_t data):data(data)
{
#ifdef _DEBUG
    assert(Fields.State == GVM::RasterizerStateDesc::RasterizerStateMask);
#endif
}

Compressed::RasterizerStateDesc::RasterizerStateDesc(GVM::RasterizerStateDesc desc)
{
      Fields.State = desc.State;
    
      Fields.FillMode  = to_underlying(desc.FillMode);
      Fields.CullMode  = to_underlying(desc.CullMode);
      
      Fields.FrontCounterClockwise = desc.FrontCounterClockwise;
      
      Fields.DepthClipEnable        = desc.DepthClipEnable;
      Fields.ScissorEnable          = desc.ScissorEnable;
      //Fields.MultisampleEnable      = desc.MultisampleEnable;
      //Fields.AntialiasedLineEnable  = desc.AntialiasedLineEnable;
}

Compressed::RasterizerStateDesc::RasterizerStateDesc(const RasterizerStateDesc& desc) :data(desc.data){}
Compressed::RasterizerStateDesc::RasterizerStateDesc(RasterizerStateDesc&& desc) noexcept :data(desc.data){}

Compressed::RasterizerStateDesc& Compressed::RasterizerStateDesc::operator=(const RasterizerStateDesc& desc)
{
    this->data = desc.data;
    return *this;
}

Compressed::RasterizerStateDesc& Compressed::RasterizerStateDesc::operator=(RasterizerStateDesc&& desc) noexcept
{
    this->data = desc.data;
    return *this;
}

inline RasterizerStateDesc::RasterizerStateDesc(): BaseStateDesc(RasterizerStateMask) {}

RasterizerStateDesc::RasterizerStateDesc(uint64_t descriptor): RasterizerStateDesc(CompressedType(descriptor)) {}

RasterizerStateDesc::RasterizerStateDesc(CompressedType descriptor): BaseStateDesc(0)
{
    
    State = descriptor.Fields.State;

#ifdef _DEBUG
    assert(State == RasterizerStateMask);
#endif
    
    assert(State == RasterizerStateMask);
    
    FillMode = to_enum<EFillMode>(descriptor.Fields.FillMode);
    CullMode = to_enum<ECullMode>(descriptor.Fields.CullMode);
    
    FrontCounterClockwise  =  descriptor.Fields.FrontCounterClockwise;
    
    //DepthBias = GetBiteValue(descriptor, shift, 32);
    //DepthBiasClamp = GetBiteValue(descriptor, shift, 16);
    //SlopeScaledDepthBias = GetBiteValue(descriptor, shift, 16);
    
    DepthClipEnable = descriptor.Fields.DepthClipEnable;
    ScissorEnable = descriptor.Fields.ScissorEnable;
    //MultisampleEnable = GetBiteValue(descriptor, shift, 1);
    //AntialiasedLineEnable = GetBiteValue(descriptor, shift, 1);
}

RasterizerStateDesc::CompressedType RasterizerStateDesc::Compress() const
{
    return *this;
}

//
// uint64_t RasterizerStateDesc::ToUInt() const {
//     
//     uint64_t result = State;
//     
//     result = (result<<1)|to_underlying(FillMode);
//     result = (result<<2)|to_underlying(FillMode);
//     
//     result = (result<<1)|FrontCounterClockwise;
//     result = (result<<32)|DepthBias;
//     result = (result<<16)|DepthBiasClamp;
//     result = (result<<16)|SlopeScaledDepthBias;
//     
//     result = (result<<1)|DepthClipEnable;
//     result = (result<<1)|ScissorEnable;
//     result = (result<<1)|MultisampleEnable;
//     result = (result<<1)|AntialiasedLineEnable;
//
//     return result;
//     
// }

