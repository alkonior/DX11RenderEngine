﻿#pragma once
#include <fstream>
#include <string>
#include <istream>
#include "IRenderer.h"
#include "CoreRenderSystem/BaseRenderSystem.h"
#include "CoreRenderSystem/PipelineFactory.h"


class BaseRenderPass {
protected:
    //Renderer::IRenderer* renderer;
    struct BaseRendererParams{
        const char* shaderName;
        BaseRenderSystem& renderSystem;
    };
    
    const BaseRendererParams baseRendererParams;
    Renderer::IRenderer * renderDevice;

    
    PipelineFactory* factory = nullptr;
    //StateProvider* provider = nullptr;

    public://renderer(Renderer::IRenderer::renderer),
    
    
    BaseRenderPass(const BaseRendererParams& in):
    baseRendererParams(in),
    renderDevice(in.renderSystem.pRenderer)
    { };

    void Init(const char* dirr, Renderer::IStateProvider* provider){
        
        std::string file = std::string(dirr) + baseRendererParams.shaderName;
        std::ifstream input( file.c_str(), std::ios::binary );
        std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(input), {});
        auto data = buffer.data();
        auto size = buffer.size();
        
        if (factory != nullptr) {
            //delete provider;
            delete factory;
        }
        
        factory = new PipelineFactory(renderDevice, provider, data, size);
    };
    
public:
    
    virtual void Resize() = 0;
    virtual void Init(const char* dirr) = 0;
    
    virtual void PreRender() = 0;
    virtual void PostRender() = 0;

    virtual ~BaseRenderPass() { delete factory; };
    
};