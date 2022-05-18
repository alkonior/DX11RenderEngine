#pragma once
#include "BaseStructures\GVMEnums.h"
#include "BaseStructures\IRenderClasses.h"

namespace GVM {


struct DepthStencilViewDesc {
    IResource*          resource;
    EDepthFormat        format;
    EDsvFlags           flag;
    uint8_t             SubResource;
};

struct VertexBufferViewDesc
{
    IVertexBuffer*     buffer;
    uint32_t           size;
    uint32_t           stride;;
};
    
struct IndexBufferViewDesc
{
    IIndexBuffer*      buffer;
    EFormat            format;
    uint32_t           size;
};
    
struct ConstBufferViewDesc
{
    IConstBuffer*      buffer;
    uint32_t           size;
};

}


