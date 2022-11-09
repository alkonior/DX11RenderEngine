#pragma once
#include "Utils/DrawData.h"
#include "Utils/ModelMesh.h"




struct DiffuseData {
    bool isTextured = false;
    DiffuseData():isTextured(false) {};
    DiffuseData(const DiffuseData& data):
    isTextured(data.isTextured),
    dummy1(data.dummy1),
    dummy2(data.dummy2)
    {};
    union {
        struct {
            uint64_t dummy1;
            uint64_t dummy2;
        };
        uint64_t textureId;
        TextureData::Color color = TextureData::Color(0,255,0,0);
    };
};

struct NormalData {
    bool isTextured = false;
    NormalData(const NormalData& data):
    isTextured(data.isTextured),
    dummy1(data.dummy1),
    dummy2(data.dummy2)
    {}
    union {
        struct {
            uint64_t dummy1;
            uint64_t dummy2;
        };
        size_t textureId;
        Float3Data::Color normal = {1,1,1};
    };
};

struct RoughnessData {
    bool isTextured = false;
    RoughnessData(const RoughnessData& data):
    isTextured(data.isTextured),
    dummy1(data.dummy1)
    {}
    union {
        struct {
            uint64_t dummy1;
        };
        size_t textureId;
        FloatData::Color roughness = {1};
    };
};

struct MetallicData {
    bool isTextured = false;
    MetallicData(const MetallicData& data):
    isTextured(data.isTextured),
    dummy1(data.dummy1)
    {}
    union {
        struct {
            uint64_t dummy1;
        };
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

struct OpaqueFlags {
    ModelsFlags flags;
};

struct OpaqueModelDrawData {
    OpaqueMeshId modelId;

    Transform world;
    Transform oldWorld;
    
    float MetallicMult = 1;
    float RoughnessMult = 1;
    
    
    MaterialData materialData;
    
    OpaqueFlags flags;
};
