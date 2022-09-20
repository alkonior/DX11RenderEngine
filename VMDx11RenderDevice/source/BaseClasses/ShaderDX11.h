#pragma once
#include "IRenderDevice.h"
namespace GVM {
class ShaderDX11 : public IRenderDevice::IShader
{
public:
    void Place(void* ptr) const override;
    ~ShaderDX11() override;
    uint8_t data = 5;
};
}