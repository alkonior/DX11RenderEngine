#ifdef __cplusplus
#pragma once
#include "../../Utils/TransformUtils.h"
#include "../../CoreShaderInclude.h"
#endif

struct BeamCosntBuffer
{
    float3 point[2];
    float4 color;
    float width;
};


STRUCTURE(1, BeamCosntBuffer, beamCosntBuffer)