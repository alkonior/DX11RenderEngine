#pragma once
#include "BlendState.h"
#include "BaseStructures\IRenderClasses.h"

namespace GVM {
namespace Compressed {
    struct RenderTargetDesc {
        IRenderTargetView* rtv;
        uint64_t BlendState;
    };
}

    struct RenderTargetDesc {
        using CompressedType = Compressed::RenderTargetDesc;
    
        IRenderTargetView* rtv;
        BlendStateDesc BlendState;
    };

}

