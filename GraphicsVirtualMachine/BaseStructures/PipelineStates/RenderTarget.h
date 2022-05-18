#pragma once
#include "BlendState.h"
#include "BaseStructures\IRenderClasses.h"

namespace GVM {
namespace Compressed {
    struct RenderTargetDesc {
        uint64_t BlendState;
        IRenderTargetView* rtv;
    };
}

    struct RenderTargetDesc {
        using CompressedType = Compressed::RenderTargetDesc;
        IRenderTargetView* rtv;
        BlendStateDesc BlendState;
    };

}

