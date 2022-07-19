#include "IndexBufferViewD3D11.h"

using namespace GVM;

HRESULT IndexBufferViewD3D11::QueryInterface(const IID& riid, void** ppvObject)
{
    *ppvObject = nullptr;
    return E_NOINTERFACE;
}

ULONG IndexBufferViewD3D11::AddRef()
{
    refCounter++;
    return refCounter.load();
}

ULONG IndexBufferViewD3D11::Release()
{
    refCounter--;
    if (refCounter.load() == 0)
        delete this;
    return 0;
}
IndexBufferViewD3D11::IndexBufferViewD3D11(const GVM::IndexBufferViewDesc& desc, ID3D11Buffer* buffer)
    :indexBuffer(buffer), format(DXGI_FORMAT_UNKNOWN)
{
    switch (desc.Format)
    {
    case EFormat::FORMAT_R16_UINT: format = DXGI_FORMAT_R16_UINT; break;
    case EFormat::FORMAT_R32_UINT: format = DXGI_FORMAT_R32_UINT; break;
    }

#ifdef _DEBUG
    assert(format!=DXGI_FORMAT_UNKNOWN);
#endif
    
}
