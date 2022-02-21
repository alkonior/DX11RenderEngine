#ifdef __cplusplus
#pragma once
#include "TransformUtils.h"
#include "CoreShaderInclude.h"
#endif

struct UPTranformCosntBuffer {
    matrix world;
    matrix view;
    matrix projection;
};

struct UPDataCosntBuffer {
    float4 color;
    //float2 texOffset;
};

STRUCTURE(0, UPTranformCosntBuffer, UPTransform)
STRUCTURE(1, UPDataCosntBuffer, UPExtraData)