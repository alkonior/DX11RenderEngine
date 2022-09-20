#pragma once

#include "GVMBaseStructures/ResourceViews/BaseViews.h"
#include "winHandler.h"

namespace GVM{
class ConstBufferViewD3D11 : public IUnknown{
    std::atomic_int refCounter;
public:
    HRESULT __stdcall QueryInterface(const IID& riid, void** ppvObject) override;
    ULONG __stdcall AddRef() override;
    ULONG __stdcall Release() override;

    explicit ConstBufferViewD3D11(const GVM::ConstBufferViewDesc& desc, ID3D11Buffer* buffer);
    ID3D11Buffer* constBuffer;
    //DXGI_FORMAT format;
};
}