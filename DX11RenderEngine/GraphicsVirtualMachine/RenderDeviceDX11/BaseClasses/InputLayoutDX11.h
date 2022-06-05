#pragma once
#include "../../VirtualMachine/IRenderDevice.h"

namespace GVM {
    
class InputLayoutDX11: public GVM::IRenderDevice::IInputLayout
{
public:
    void Place(void* ptr) const override;
    ~InputLayoutDX11() override;

    uint32_t data[5] = {1,2,3,4,5};
};

}