#pragma once
#include "GVMBaseStructures\GVMEnums.h"
#include "GVMBaseStructures\IRenderClasses.h"

namespace GVM
{
struct DepthStencilViewDesc
{
    Resource* Resource;
    EDepthFormat Format;
    EDsvFlags Flag;
};

struct VertexBufferViewDesc
{
    VertexBuffer* Buffer;
    uint32_t Stride;
    uint32_t Offset;
    uint32_t Size;

    VertexBufferViewDesc(
        VertexBuffer* Buffer,
        uint32_t Stride,
        uint32_t Offset,
        uint32_t Size):
        Buffer(Buffer),
        Stride(Stride),
        Offset(Offset),
        Size(Size)
    {
    }
};

struct IndexBufferViewDesc
{
    IndexBuffer* Buffer;
    EFormat Format;
    uint32_t Offset;
    uint32_t Size;

    IndexBufferViewDesc(
        IndexBuffer* Buffer,
        EFormat Format,
        uint32_t Offset,
        uint32_t Size):
        Buffer(Buffer),
        Format(Format),
        Offset(Offset),
        Size(Size)
    {
    }
};

struct ConstBufferViewDesc
{
    ConstBuffer* Buffer;
    uint32_t Offset;
    uint32_t Size;

    ConstBufferViewDesc(
        ConstBuffer* Buffer,
        uint32_t Offset,
        uint32_t Size):
        Buffer(Buffer),
        Offset(Offset),
        Size(Size)
    {
    }
};
}
