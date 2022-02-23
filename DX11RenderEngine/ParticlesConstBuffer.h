#ifdef __cplusplus
#pragma once
#include "TransformUtils.h"
#include "CoreShaderInclude.h"
#endif

struct ParticlesCosntBuffer {
    //matrix world;
    matrix view;
    matrix projection;
    float4 color;
};

STRUCTURE(0, ParticlesCosntBuffer, ParticlesCB)