#ifdef __cplusplus
#pragma once
#include "TransformUtils.h"
#include "CoreRenderSystem/CoreShaderInclude.h"
#endif

struct SkyboxCosntBuffer
{
    matrix skyboxView;
};

STRUCTURE(1, SkyboxCosntBuffer, skyboxCosntBuffer)