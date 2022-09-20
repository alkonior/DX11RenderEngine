#pragma once
#include "GVMBaseStructures/ResourceViews/BaseViews.h"
#include "winHandler.h"

namespace GVM {

class VertexBufferViewD3D11 : public IUnknown{
    std::atomic_int refCounter;
public:
    HRESULT __stdcall QueryInterface(const IID& riid, void** ppvObject) override;
    ULONG __stdcall AddRef() override;
    ULONG __stdcall Release() override;

    explicit VertexBufferViewD3D11(const GVM::VertexBufferViewDesc& desc, ID3D11Buffer* buffer);
    ID3D11Buffer* vertexBuffer;
    UINT Stride;
    UINT Offset;

};
}
