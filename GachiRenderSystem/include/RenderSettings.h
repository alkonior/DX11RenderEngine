#pragma once

struct WindowSettings
{
    size_t windowWidth;
    size_t windowHeight;
    size_t viewportWidth;
    size_t viewportHeight;
};

struct  RenderEngineCoreSettings {
    void* hWnd1;
    void* hWnd2;
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

struct RenderSettings
{
    const char* shadersDirr;
    WindowSettings windowSettings;
    TAASettings taaSettings;
};
