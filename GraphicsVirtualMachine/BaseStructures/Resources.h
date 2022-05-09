#pragma once
#include "GVMEnums.h"
namespace GVM {

struct SampleDesc {
    uint32_t Count = 1;
    uint32_t Quality = 0;
};

struct BufferDesc {
    uint32_t             Width;
    uint32_t             BindFlags;
    void*                initialData;
};

struct VertexBufferDesc : BufferDesc {
//todo
};

struct IndexBufferDesc : BufferDesc {
//todo
};

struct Texture1DDesc : BufferDesc {
    EFormat          Format;
    uint16_t             ArraySize;
    uint16_t             MipLevels;
    SampleDesc           SampleDesc;
    uint32_t             UsageType;
};

struct Texture2DDesc : Texture1DDesc {
    uint32_t             Height;
};

struct Texture3DDesc : Texture2DDesc {
    uint16_t             Depth;
};

struct ResourceViewDesc {
    EFormat     Format;
    //todo
};

}