#pragma once
#include "Utils/DrawData.h"
#include "Utils/ModelData.h"

struct OpaqueFlags {
    
};
namespace Hidden {
struct TextureData {
    bool isTextured = false;
    union my_union {
        size_t textureId;
        ::TextureData::Color color;
    };
};
struct FloatTextureData {
    
};
}

struct DiffuseData : TextureData {
    
};

struct NormalData {
    
};

struct RoughnessData {
    
};

struct MetallicData {
    
};

struct OpaqueModelDrawData {
    OpaqueMeshId Id;

    
    
};
