#pragma once
#include "../../BaseStructures/GVMEnums.h"

namespace GVM
{
    //TODO MipLevels too ahrd to implement, may be later
struct ResourceDesc
{
    EResourceDimension       Dimension = EResourceDimension::RESOURCE_DIMENSION_UNKNOWN;
    EFormat                  Format = EFormat::FORMAT_UNKNOWN;
    uint32_t                 Width = 0;
    uint32_t                 Height= 0;
    uint16_t                 DepthOrArraySize= 0;
    //uint16_t                 MipLevels;
    void*                    initialData = nullptr;
};


 struct BufferResourceDesc
 {
     uint32_t        Size;
     void*           initialData;
 };
    
    
struct Texture1DResourceDesc
{
    uint32_t                 Width;
    uint16_t                 Array;
    //uint16_t                 MipLevels;
    EFormat                  Format;
    void*                    initialData;
};
    
struct Texture2DResourceDesc
{
    uint32_t                 Width;
    uint32_t                 Height;
    uint16_t                 Array;
    //uint16_t                 MipLevels;
    EFormat                  Format;
    void*                    initialData;
};
    
struct TextureCubeResourceDesc
{
    uint32_t                 Width;
    uint32_t                 Height;
    uint16_t                 Array;
    //uint16_t                 MipLevels;
    EFormat                  Format;
    void*                    initialData;
};
    
struct Texture3DResourceDesc
{
    uint32_t                 Width;
    uint32_t                 Height;
    uint16_t                 Depth;
    //uint16_t                 MipLevels;
    EFormat                  Format;
    void*                    initialData;
};

}