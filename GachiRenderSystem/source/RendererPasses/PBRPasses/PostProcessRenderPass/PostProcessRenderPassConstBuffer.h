#ifdef __cplusplus
#pragma once
#include "TransformUtils.h"
#include "CoreRenderSystem/CoreShaderInclude.h"
#endif


struct PostProcessConstantData
{
    float lum;
};

STRUCTURE(1, PostProcessConstantData, postProcessCosntBuffer)