#include "RasterizerState.h"
#include <cassert>
#include "BaseDefines.h"

using namespace GVM;

const RasterizerStateDesc RasterizerStateDesc::Default;

Compressed::RasterizerStateDesc::RasterizerStateDesc(uint64_t data):data(data)
{
#ifdef _DEBUG
    assert(Fields.RasterizerMask == GVM::RasterizerStateDesc::RasterizerStateMask);
#endif
}

Compressed::RasterizerStateDesc::RasterizerStateDesc(GVM::RasterizerStateDesc desc)
{
      Fields.RasterizerMask = desc.State;
    
      Fields.FillMode  = to_underlying(desc.FillMode);
      Fields.CullMode  = to_underlying(desc.FillMode);
      
      Fields.FrontCounterClockwise = desc.FrontCounterClockwise;
      
      Fields.DepthClipEnable        = desc.DepthClipEnable;
      Fields.ScissorEnable          = desc.ScissorEnable;
      //Fields.MultisampleEnable      = desc.MultisampleEnable;
      //Fields.AntialiasedLineEnable  = desc.AntialiasedLineEnable;
}

Compressed::RasterizerStateDesc::RasterizerStateDesc(const RasterizerStateDesc& desc) :data(desc.data)
{}

Compressed::RasterizerStateDesc::RasterizerStateDesc(RasterizerStateDesc&& desc) noexcept :data(desc.data)
{
}

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

inline RasterizerStateDesc::RasterizerStateDesc() : BaseStateDesc(RasterizerStateMask) {}

RasterizerStateDesc::RasterizerStateDesc(uint64_t descriptor) : BaseStateDesc(0) {
    
   // uint8_t shift = 0;
//
   // State = GetBiteValue(descriptor, shift, 3);
   // assert(State == RasterizerStateMask);
   // 
   // FillMode = to_enum<EFillMode>(GetBiteValue(descriptor, shift, 1));
   // CullMode = to_enum<ECullMode>(GetBiteValue(descriptor, shift, 2));
   // 
   // FrontCounterClockwise  = GetBiteValue(descriptor, shift, 1);
   // 
   // DepthBias = GetBiteValue(descriptor, shift, 32);
   // DepthBiasClamp = GetBiteValue(descriptor, shift, 16);
   // SlopeScaledDepthBias = GetBiteValue(descriptor, shift, 16);
   // 
   // DepthClipEnable = GetBiteValue(descriptor, shift, 1);
   // ScissorEnable = GetBiteValue(descriptor, shift, 1);
   // MultisampleEnable = GetBiteValue(descriptor, shift, 1);
   // AntialiasedLineEnable = GetBiteValue(descriptor, shift, 1);
    

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

