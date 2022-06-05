#pragma once

namespace GVM {

struct ViewportDesc {
    using CompressedType = ViewportDesc;
    float TopLeftX;
    float TopLeftY;
    float Width;
    float Height;
    float MinDepth;
    float MaxDepth;
};

namespace Compressed
{
    typedef typename GVM::ViewportDesc ViewportDesc;
    
}

}
