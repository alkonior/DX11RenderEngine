#pragma once
#include "IRenderer.h"
//#include "RenderFlags.h"
#include <map>
#include <vector>

struct PipelineFactoryFlags {

    PipelineFactoryFlags(): flags(0) {}
    PipelineFactoryFlags(uint64_t flags): flags(flags), useShaders(0) {}
    PipelineFactoryFlags(uint64_t flags, uint8_t useShaders): flags(flags), useShaders(useShaders) {}
    PipelineFactoryFlags(uint32_t definesFlags, uint32_t pipelineFlags): definesFlags(definesFlags), pipelineFlags(pipelineFlags), useShaders(0) {}
    PipelineFactoryFlags(uint32_t definesFlags, uint32_t pipelineFlags, uint8_t useShaders): definesFlags(definesFlags), pipelineFlags(pipelineFlags), useShaders(useShaders) {}
    PipelineFactoryFlags(uint32_t flags): definesFlags(flags), pipelineFlags(flags), useShaders(0) {}

    union {
        uint32_t flags = 0;
        struct alignas(0) {
            uint16_t definesFlags;
            uint16_t pipelineFlags;
        };
    };
    uint8_t useShaders : 4;
};

class PipelineFactory {

    Renderer::IRenderer* renderDevice;
    Renderer::IStateProvider* provider;

    void* shaderData;
    size_t dataSize;
    uint16_t compileFlags;

    const Renderer::ShaderDefines* defines;
    size_t defineCount;

    std::map<uint32_t, Renderer::Pipeline*> dictinaryPipeline;
    std::map<uint32_t, Renderer::PipelineShaders*> dictinaryShaders;
    std::map<std::pair<uint32_t, const char*>, Renderer::PipelineShaders*> computeDictinary;



    PipelineFactory(Renderer::IStateProvider* provider, const Renderer::ShaderDefines* defines, size_t defineCount, uint16_t compileFlags);
public:
    PipelineFactory(
        Renderer::IRenderer* renderDevice,
        Renderer::IStateProvider* provider,
        void* shaderData,
        size_t dataSize);

    std::vector<Renderer::ShaderDefines> GetDefines(uint32_t definesFlags);

    Renderer::PipelineState GetState(PipelineFactoryFlags definesFlags);
    Renderer::PipelineState GetComputeState(uint32_t definesFlags, const char* name);


    ~PipelineFactory();
};
