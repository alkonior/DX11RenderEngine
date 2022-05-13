#pragma once
#include "BaseStructures\GVMEnums.h"
#include "BaseStructures\IRenderClasses.h"

namespace GVM {



struct Tex1DDesc {
    uint32_t  MostDetailedMip;
    uint32_t  MipLevels;
    float ResourceMinLODClamp;
};

struct Tex1DArrayDesc {
    uint32_t  MostDetailedMip;
    uint32_t  MipLevels;
    uint32_t  FirstArraySlice;
    uint32_t  ArraySize;
    float ResourceMinLODClamp;
};

struct TEX2DDesc {
    uint32_t  MostDetailedMip;
    uint32_t  MipLevels;
    uint32_t  PlaneSlice;
    float ResourceMinLODClamp;
};

struct Tex2DArrayDesc {
    uint32_t  MostDetailedMip;
    uint32_t  MipLevels;
    uint32_t  FirstArraySlice;
    uint32_t  ArraySize;
    uint32_t  PlaneSlice;
    float ResourceMinLODClamp;
};

struct Tex3DDesc {
    uint32_t  MostDetailedMip;
    uint32_t  MipLevels;
    float ResourceMinLODClamp;
};

struct TexcubeDesc {
    uint32_t  MostDetailedMip;
    uint32_t  MipLevels;
    float ResourceMinLODClamp;
};

struct TexcubeArrayDesc {
    uint32_t  MostDetailedMip;
    uint32_t  MipLevels;
    uint32_t  First2DArrayFace;
    uint32_t  NumCubes;
    float ResourceMinLODClamp;
};

struct RenderTargetViewDesc {
    IResource*              resource;
    EFormat                 Format;
    EResourceViewDimension  Dimension;
    union{
        Tex1DDesc           T1Desc;
        Tex1DArrayDesc      T1ADesc;
        TEX2DDesc           T2Desc;
        Tex2DArrayDesc      T2ADesc;
        Tex3DDesc           T3Desc;
        TexcubeDesc         TcDesc;
        TexcubeArrayDesc    TcADesc;
    };
};

struct ShaderResourceViewDesc {
    IResource*              Resource;
    EFormat                 Format;
    ERtViewDimension        Dimension;
    union{
        Tex1DDesc           T1Desc;
        Tex1DArrayDesc      T1ADesc;
        TEX2DDesc           T2Desc;
        Tex2DArrayDesc      T2ADesc;
        Tex3DDesc           T3Desc;
        TexcubeDesc         TcDesc;
        TexcubeArrayDesc    TcADesc;
    };
};


}


