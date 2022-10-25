#pragma once
#include "TransformUtils.h"
#include "TextureData.h"

struct  RenderEngineInitStruct {
    void* hWnd1;
    void* hWnd2;
    size_t width; size_t height;
};




struct UIDrawData {

    int x; int y;
    int width; int height; 
    int top; int left; int texW; int texH;
    float4 color = { 0,0,0,0 };
    uint64_t flag;
};

union ModelsFlags{
    ModelsFlags() {isVisible = 1;}
    uint32_t flags;
    struct {
        uint8_t isRed : 1 ;  
        uint8_t isVisible : 1;  
        uint8_t isColored : 1;  
        uint8_t isWireframe : 1;
    } ;
};

struct ModelDrawData {
    ModelDrawData() = default;
    
    ModelDrawData(size_t modelId, size_t textureId, Transform position):
    modelId(modelId),
    textureId(textureId),
    oldPosition(position),
    newPosition(position)
    {};
    
    ModelDrawData(size_t modelId, Transform position):
    modelId(modelId),
    textureId(0),
    oldPosition(position),
    newPosition(position)
    {
        flags.isRed = 1;
    };
    
    ModelDrawData(size_t modelId, float4 color, Transform position):
    modelId(modelId),
    textureId(0),
    oldPosition(position),
    newPosition(position)
    {
        flags.isColored = 1;
    };
    
    size_t modelId = 0;
    size_t textureId = 0;
    
    Transform oldPosition;
    Transform newPosition;
    
    float4 color;
    ModelsFlags flags;
};

struct MeshHashData {
    union {
        uint64_t id = 0;
        struct {
            uint32_t vertexOffset : 24;
            uint32_t indexOffset : 24;
            int numElem : 16;
        };
    };
};


struct UPDrawData {
    //UPModelData model;
    Transform position;
    float2 texOffset;
    float4 light;

    bool dynamic;

    bool dynamicLight = false;
    ImageUpdate* lightUpdate = nullptr;

    uint64_t flags;
};


struct ParticlesDrawData {
    uint64_t flags;
};
