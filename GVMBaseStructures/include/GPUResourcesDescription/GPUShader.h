#pragma once
#include "IRenderDevice.h"

namespace GVM
{
    

class GPUShader
{
public:
    GPUShader(const ShaderDesc& desc, int id);
    ShaderDesc description;
    IRenderDevice::IShader* shader = nullptr;
    int id = -1;
};

    
}
