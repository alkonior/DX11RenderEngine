#pragma once
#include "../GraphicsBase.h"
#include "IRenderer\IRenderer.h"




class BaseRenderer
{
protected:
    //Renderer::IRenderer* renderer;
    const char* shaderName;


public://renderer(Renderer::IRenderer::renderer),
    BaseRenderer(const char* shaderName): shaderName(shaderName)
    {
        
    };


    void Init(const char* dirr){
        std::string file = std::string(dirr) + shaderName;
        std::ifstream input( file.c_str(), std::ios::binary );
        std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(input), {});
        auto data = buffer.data();
        auto size = buffer.size();
        Init((void*)data, size);
    };
    
    virtual void Init(void* shaderData, size_t dataLenght) = 0;
    
    virtual void Clear() = 0;
    virtual void Clear(GraphicsBase& gfx) { Clear(); };

    virtual ~BaseRenderer() {};
};