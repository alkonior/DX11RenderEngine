#pragma once
#include "BaseStructures\GVMEnums.h"
#include "BaseStructures\IRenderClasses.h"

namespace GVM {


struct DepthStencilViewDesc {
    Resource*          Resource;
    EDepthFormat       Format;
    EDsvFlags          Flag;
};

struct VertexBufferViewDesc
{
    VertexBuffer*     Buffer;
    uint32_t          Size;
    uint32_t          Stride;
};
    
struct IndexBufferViewDesc
{
    IndexBuffer*      Buffer;
    EFormat           Format;
    uint32_t          Size;
};
    
struct ConstBufferViewDesc
{
    ConstBuffer*      Buffer;
    uint32_t          Size;
};

}


