#pragma once
#include "BaseStructures/GVMEnums.h"

namespace GVM
{
    

struct BufferResourceDesc
{
    uint64_t                 Size;
};
    
struct VertexBufferDesc : BufferResourceDesc
{};
    
struct IndexBufferDesc : BufferResourceDesc
{};
struct ConstBufferDesc : BufferResourceDesc
{};
    
struct TextureResourceDesc
{
    EResourceDimension       Dimension;
    uint64_t                 Width;
    uint32_t                 Height;
    uint16_t                 DepthOrArraySize;
    uint16_t                 MipLevels;
    EFormat                  Format;
};
    
struct Texture1DResourceDesc
{
    uint64_t                 Width;
    uint64_t                 array;
    uint16_t                 MipLevels;
    EFormat                  Format;
};
    
struct Texture2DResourceDesc
{
    uint64_t                 Width;
    uint64_t                 Height;
    uint64_t                 Array;
    uint16_t                 MipLevels;
    EFormat                  Format;
};
    
struct TextureCubeResourceDesc
{
    uint64_t                 Width;
    uint64_t                 Height;
    uint16_t                 MipLevels;
    EFormat                  Format;
};
    
struct Texture3DResourceDesc
{
    uint64_t                 Width;
    uint64_t                 Height;
    uint64_t                 Depth;
    uint16_t                 MipLevels;
    EFormat                  Format;
};

}