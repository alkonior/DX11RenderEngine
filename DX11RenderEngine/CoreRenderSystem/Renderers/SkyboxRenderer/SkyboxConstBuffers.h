#ifdef __cplusplus
#pragma once
#include "../../Utils/TransformUtils.h"
#include "../../CoreShaderInclude.h"
#endif

struct SkyboxCosntBuffer
{
    matrix skyboxView;
};

STRUCTURE(1, SkyboxCosntBuffer, skyboxCosntBuffer)