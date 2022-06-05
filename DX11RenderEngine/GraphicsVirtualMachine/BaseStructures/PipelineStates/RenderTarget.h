#pragma once
#include "BlendState.h"
#include "../../BaseStructures\IRenderClasses.h"

namespace GVM {
namespace Compressed {
    struct RenderTargetDesc {
        Compressed::BlendStateDesc BlendState;
    };
}

    struct RenderTargetDesc {
        using CompressedType = Compressed::RenderTargetDesc;
        RenderTargetView* rtv;
        BlendStateDesc BlendState;
    };

}

