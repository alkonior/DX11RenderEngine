#ifdef __cplusplus
#pragma once
#include "TransformUtils.h"
#include "CoreShaderInclude.h"
#endif

struct ParticlesCosntBuffer {
    //matrix world;
    matrix view;
    matrix projection;
};

STRUCTURE(0, ParticlesCosntBuffer, ParticlesCB)