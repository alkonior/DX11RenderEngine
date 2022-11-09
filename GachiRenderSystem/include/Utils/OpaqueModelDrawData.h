#pragma once
#include "Utils/DrawData.h"
#include "Utils/ModelData.h"

struct OpaqueFlags {
    
};


struct DiffuseData {
    bool hasTextured = false;
    union my_union {
        size_t textureId;
        TextureData::Color color = TextureData::Color(0,255,0,0);
    };
};

struct NormalData {
    bool isTextured = false;
    union my_union {
        size_t textureId;
        Float3Data::Color normal = {1,1,1};
    };
};

struct RoughnessData {
    bool isTextured = false;
    union my_union {
        size_t textureId;
        FloatData::Color roughness = {1};
    };
};

struct MetallicData {
    bool isTextured = false;
    union my_union {
        size_t textureId;
        FloatData::Color metallic = {0};
    };
};
struct MaterialData {
    
    DiffuseData   diffuseData;
    NormalData    normalData;
    RoughnessData roughnessData;
    MetallicData  metallicData;
};

struct OpaqueModelDrawData {
    OpaqueMeshId Id;
    
    MaterialData materiaData;
    
    ModelsFlags flags;
};
