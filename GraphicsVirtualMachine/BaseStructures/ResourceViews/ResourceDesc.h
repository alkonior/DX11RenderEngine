#pragma once
#include "BaseStructures/GVMEnums.h"

namespace GVM
{
    

struct BufferResourceDesc
{
    uint32_t        Size;
    void*           initialData;
};
    
    
struct TextureResourceDesc
{
    EResourceDimension       Dimension;
    EFormat                  Format;
    uint32_t                 Width;
    uint32_t                 Height;
    uint16_t                 DepthOrArraySize;
    uint16_t                 MipLevels;
    void*                    initialData;
};
    
struct Texture1DResourceDesc
{
    uint64_t                 Width;
    uint64_t                 array;
    uint16_t                 MipLevels;
    EFormat                  Format;
    void*                    initialData;
};
    
struct Texture2DResourceDesc
{
    uint64_t                 Width;
    uint64_t                 Height;
    uint64_t                 Array;
    uint16_t                 MipLevels;
    EFormat                  Format;
    void*                    initialData;
};
    
struct TextureCubeResourceDesc
{
    uint64_t                 Width;
    uint64_t                 Height;
    uint16_t                 MipLevels;
    EFormat                  Format;
    void*                    initialData;
};
    
struct Texture3DResourceDesc
{
    uint64_t                 Width;
    uint64_t                 Height;
    uint64_t                 Depth;
    uint16_t                 MipLevels;
    EFormat                  Format;
    void*                    initialData;
};

}