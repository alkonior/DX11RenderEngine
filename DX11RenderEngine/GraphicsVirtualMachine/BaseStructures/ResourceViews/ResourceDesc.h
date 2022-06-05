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
     uint32_t        Size = 0;
     void*           initialData = nullptr;
 };
    
    
struct Texture1DResourceDesc
{
    uint32_t                 Width = 0;
    uint16_t                 Array = 0;
    //uint16_t                 MipLevels;
    EFormat                  Format= EFormat::FORMAT_UNKNOWN;
    void*                    initialData= nullptr;
};
    
struct Texture2DResourceDesc
{
    uint32_t                 Width = 0;
    uint32_t                 Height = 0;
    uint16_t                 Array = 0;
    //uint16_t                 MipLevels;
    EFormat                  Format= EFormat::FORMAT_UNKNOWN;
    void*                    initialData= nullptr;
};
    
struct TextureCubeResourceDesc
{
    uint32_t                 Width = 0;
    uint32_t                 Height = 0;
    uint16_t                 Array = 0;
    //uint16_t                 MipLevels;
    EFormat                  Format= EFormat::FORMAT_UNKNOWN;
    void*                    initialData;
};
    
struct Texture3DResourceDesc
{
    uint32_t                 Width = 0;
    uint32_t                 Height = 0;
    uint16_t                 Depth = 0;
    //uint16_t                 MipLevels;
    EFormat                  Format = EFormat::FORMAT_UNKNOWN;
    void*                    initialData = nullptr;
};

}