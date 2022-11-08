#ifdef __cplusplus
#pragma once
#include "TransformUtils.h"
#include "CoreRenderSystem/CoreShaderInclude.h"
#endif

struct OpaqueCosntBuffer {
    matrix world;
    matrix oldWorld;

    
};

STRUCTURE(1, OpaqueCosntBuffer, opaqueCosntBuffer)