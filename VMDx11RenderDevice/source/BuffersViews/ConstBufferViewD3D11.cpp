#include "pch.h"
#include "ConstBufferViewD3D11.h"

using namespace GVM;

HRESULT ConstBufferViewD3D11::QueryInterface(const IID& riid, void** ppvObject)
{
    *ppvObject = nullptr;
    return E_NOINTERFACE;
}

ULONG ConstBufferViewD3D11::AddRef()
{
    refCounter++;
    return refCounter.load();
}

ULONG ConstBufferViewD3D11::Release()
{
    refCounter--;
    if (refCounter.load() == 0)
        delete this;
    return 0;
}
ConstBufferViewD3D11::ConstBufferViewD3D11(const GVM::ConstBufferViewDesc& desc, ID3D11Buffer* buffer)
    :constBuffer(buffer)
{

    
}
