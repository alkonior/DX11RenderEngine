#pragma once

struct WindowSettings
{
    size_t windowWidth;
    size_t windowHeight;
    size_t viewportWidth;
    size_t viewportHeight;
};

struct  RenderEngineCoreSettings {
    void* hWnd;
    WindowSettings windowSettings;
};

struct TAASettings {
    bool markNoHistoryPixels;
    bool allowBicubicFilter;
    bool allowDepthThreshold;
    bool allowVarianceClipping;
    bool allowNeighbourhoodSampling;
    bool allowYCoCg;
    bool allowLongestVelocityVector;
    int numSamples;
    float taaStrength;
};

struct PostProcessSettings {
    float lum;
    uint8_t mode = 2;
};

struct RenderSettings
{
    const char* shadersDirr;
    WindowSettings windowSettings;
    TAASettings taaSettings;
    PostProcessSettings ppSettings;
};
