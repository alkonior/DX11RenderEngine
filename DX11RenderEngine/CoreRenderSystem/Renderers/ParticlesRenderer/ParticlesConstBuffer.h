#ifdef __cplusplus
#pragma once
#include "../../Utils/TransformUtils.h"
#include "../../CoreShaderInclude.h"
#endif

struct ParticlesCosntBuffer {
    //matrix world;
    matrix view;
    matrix projection;
};

STRUCTURE(0, ParticlesCosntBuffer, ParticlesCB)