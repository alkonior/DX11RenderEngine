#pragma once
#include "BlendState.h"
#include "BaseStructures\IRenderClasses.h"

namespace GVM {
namespace Compressed {
    struct RenderTargetDesc {
        uint64_t BlendState;
        RenderTargetView* rtv;
    };
}

    struct RenderTargetDesc {
        using CompressedType = Compressed::RenderTargetDesc;
        RenderTargetView* rtv;
        BlendStateDesc BlendState;
    };

}

