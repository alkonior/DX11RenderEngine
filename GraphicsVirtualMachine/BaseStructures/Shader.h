#pragma once
#include "GVMEnums.h"

namespace GVM {

struct ShaderDesc {
    EShaderType type;
    void* bytecode;
    uint32_t byteCodeSize;
};

}
