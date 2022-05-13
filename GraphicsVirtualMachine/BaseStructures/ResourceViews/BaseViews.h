#pragma once
#include "BaseStructures\GVMEnums.h"
#include "BaseStructures\IRenderClasses.h"

namespace GVM {

struct ConstantBufferViewDesc {
    IResource* resource;
};


struct DepthStencilViewDesc {
    IResource*          resource;
    EDepthFormat        format;
    EDsvFlags           flag;
    uint8_t             SubResource;
};



}


