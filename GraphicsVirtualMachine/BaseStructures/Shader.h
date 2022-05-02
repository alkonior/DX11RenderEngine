#pragma once
#include "GVMEnums.h"

namespace GVM {

struct ShaderDesc {
    EShaderType type;
    void* bytecode;
    uint32_t byteCodeSize;;
#ifdef _DEBUG
    const char* name;
#endif
};
struct ShaderData
{
    void* shaderData; size_t dataSize; 
    ShaderDefines* defines; size_t definesSize; 
    void* includes;
    const char* enteryPoint;
    const char* target;
    uint16_t flags
};

}
