#pragma once
#include "TransformUtils.h"

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


struct LerpModelDrawData {
    Transform oldPosition;
    Transform newPosition;
    bool isSingle;
    bool isGun;

    float alpha;
    float oldAlpha;
    size_t currentFrame;
    size_t nextFrame;
    float4 color;

    uint64_t flags;

};

struct MeshHashData {
    int vertexOffset = 0;
    int indexOffset = 0;
    int numElem = 0;
    uint8_t pt;
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
