#pragma once
#include "GVMEnums.h"

namespace GVM {

struct ShaderDesc {
    EShaderType type = EShaderType::UNKNOWN;
    
    void* bytecode = nullptr;
    uint32_t byteCodeSize = 0;
#ifdef _DEBUG
    const char* name = "";
#endif
};

}
