#pragma once
#include "GVMBaseStructures\GVMEnums.h"
#include "GVMBaseStructures\IRenderClasses.h"

namespace GVM {


struct DepthStencilViewDesc {
    Resource*          Resource;
    EDepthFormat       Format;
    EDsvFlags          Flag;
};

struct VertexBufferViewDesc
{
    VertexBuffer*     Buffer;
   // uint32_t          Size;
    uint32_t          Stride;
    uint32_t          Offset;
};
    
struct IndexBufferViewDesc
{
    IndexBuffer*      Buffer;
    EFormat           Format;
    uint32_t          Offset;
   // uint32_t          Size;
};
    
struct ConstBufferViewDesc
{
    ConstBuffer*      Buffer;
    uint32_t          Offset;
    //uint32_t          Size;
};

}


