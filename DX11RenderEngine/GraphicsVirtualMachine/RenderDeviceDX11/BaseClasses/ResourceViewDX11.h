#pragma once
#include "../../VirtualMachine\IRenderDevice.h"
namespace GVM {
class ResourceViewDX11 : public GVM::IRenderDevice::IResourceView
{
public:
    void Place(void* ptr) const override;
    ~ResourceViewDX11() override;
    uint8_t data[6] = {6,5,4,3,2,1};
};
}