#pragma once
#include "VirtualMachine/IRenderDevice.h"

namespace GVM {
class ResourceDX11 : public GVM::IRenderDevice::IResource {
public:
    void Place(void* ptr) const override;
    ~ResourceDX11() override;
public:
    uint64_t data[5] = {1,2,3,4,5};
};
}
