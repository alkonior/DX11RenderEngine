#include "VertexBufferViewD3D11.h"

using namespace GVM;


HRESULT VertexBufferViewD3D11::QueryInterface(const IID& riid, void** ppvObject)
{
    *ppvObject = nullptr;
    return E_NOINTERFACE;
}

ULONG VertexBufferViewD3D11::AddRef()
{
    refCounter++;
    return refCounter.load();
}

ULONG VertexBufferViewD3D11::Release()
{
    refCounter--;
    if (refCounter.load() == 0)
        delete this;
    return 0;
}
VertexBufferViewD3D11::VertexBufferViewD3D11(const GVM::VertexBufferViewDesc& desc, ID3D11Buffer* buffer)
    :vertexBuffer(buffer), Offset(desc.Offset), Stride(desc.Stride)
{    
}
