#pragma once
#include <fstream>
#include <string>
#include <istream>
#include "IRenderer.h"
#include "CoreRenderSystem/BaseRenderSystem.h"
#include "CoreRenderSystem/PipelineFactory.h"
#include "ref_dx11/render/IStateProvider.h"

template<class... T>
class BaseRenderPass;

template<>
class BaseRenderPass<> {
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
    
    virtual void Init(const char* dirr) = 0;
    
    virtual void Render() = 0;
    virtual void PreRender() = 0;
    virtual void PostRender() = 0;

    virtual ~BaseRenderPass() { delete factory; };
    
};

template<class T>
class BaseRenderPass<T> : public BaseRenderPass<>  {

    static_assert(!std::is_void_v<T>, "T is void");
    
    using Super =  BaseRenderPass<>;
    
    public:

    BaseRenderPass(const BaseRendererParams& in) : Super(in){ };
    
    
    virtual void Render(const T&) = 0;
    virtual void RenderIMGUI() = 0;
    void Render() final {};
    
};