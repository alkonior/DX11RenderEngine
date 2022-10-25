#include "PipelineFactory.h"

#include <d3dcompiler.h>
using namespace Renderer;

PipelineFactory::PipelineFactory(IStateProvider* provider, const ShaderDefines* defines, size_t defineCount, uint16_t compileFlags):
    provider(provider),
    defines(defines),
    defineCount(defineCount),
    compileFlags(compileFlags),
    dataSize(0),
    shaderData(nullptr),
    useShaders(0)
{}

PipelineFactory::PipelineFactory(
Renderer::IRenderer* renderDevice,
Renderer::IStateProvider* provider,
void* shaderData,
size_t dataSize):
    PipelineFactory(provider, provider->GetFactoryDescription().defines, provider->GetFactoryDescription().defineCount, provider->GetFactoryDescription().compileFlags)
{
    this->renderDevice = renderDevice;
    this->shaderData = malloc(dataSize);
    _memccpy(this->shaderData, shaderData, 1, dataSize);
    this->dataSize = dataSize;;
}


std::vector<ShaderDefines> PipelineFactory::GetDefines(uint32_t definesFlags)
{
    std::vector<ShaderDefines> result;

    for (size_t i = 0; i < defineCount; i++)
    {
        if (definesFlags & size_t(1) << i)
        {
            result.push_back(ShaderDefines{defines[i].name,defines[i].defenition});
        }
    }

    result.shrink_to_fit();

    return result;
}

PipelineState PipelineFactory::GetState(PipelineFactoryFlags definesFlags)
{
    
    PipelineState ps;
    if (dictinaryShaders.count(definesFlags.definesFlags /*int*/))
    {
        ps.shaders = dictinaryShaders[definesFlags.definesFlags];
    }else
    {
        PipelineShaders* sh = new PipelineShaders();

        auto definesArray = GetDefines(definesFlags.definesFlags);
        const char* name = provider->GetShaderName();
        sh->ps = renderDevice->CompilePixelShader(
            IRenderer::ShaderData{
                shaderData,dataSize,
                definesArray.data(),
                definesArray.size(),
                D3D_COMPILE_STANDARD_FILE_INCLUDE,
                "psIn","ps_4_0",compileFlags,
#ifdef _DEBUG
                name
#endif
            }
        );

        auto inputDescriptor = provider->GetInputLayoutDescription(definesFlags.definesFlags);
        sh->vs = renderDevice->CompileVertexShader(
            IRenderer::ShaderData{
                shaderData,dataSize,definesArray.data(),
                definesArray.size(), D3D_COMPILE_STANDARD_FILE_INCLUDE,
                "vsIn","vs_4_0",compileFlags,
#ifdef _DEBUG
                name
#endif
            }, inputDescriptor.inputLayout, inputDescriptor.inputLayoutSize
        );

        if (useShaders & Renderer::UseGeometryShader)
            sh->gs = renderDevice->CompileGeometryShader(
                IRenderer::ShaderData{
                    shaderData,dataSize,definesArray.data(),
                    definesArray.size(), D3D_COMPILE_STANDARD_FILE_INCLUDE,
                    "gsIn","gs_4_0",compileFlags,
#ifdef _DEBUG
                    name
#endif
                }
            );
        else
            sh->gs = nullptr;

        if (useShaders & Renderer::UseComputeShader)
            sh->cs = renderDevice->CompileComputeShader(
                IRenderer::ShaderData{
                    shaderData,dataSize,definesArray.data(),
                    definesArray.size(), D3D_COMPILE_STANDARD_FILE_INCLUDE,
                    "csIn","cs_4_0",compileFlags,
#ifdef _DEBUG
                    name
#endif
                }
            );
        else
            sh->cs = nullptr;





        dictinaryShaders.insert({definesFlags.definesFlags,sh});
        ps.shaders = sh;
    }
    if (dictinaryPipeline.contains(definesFlags.pipelineFlags))
    {
        ps.pipeline = dictinaryPipeline[definesFlags.pipelineFlags];
    }else
    {
        Pipeline* p = new Pipeline();
        provider->PatchPipelineState(p, definesFlags.pipelineFlags);
        ps.pipeline = p;
    }
    
    return ps;
}

PipelineState PipelineFactory::GetComputeState(uint32_t definesFlags, const char* nameShader)
{
    PipelineShaders* ps;
    auto id = std::make_pair(definesFlags, nameShader);
    if (computeDictinary.count(id))
    {
        return {computeDictinary[id], nullptr};
    }
    {
        ps = new PipelineShaders();

        auto definesArray = GetDefines(definesFlags);
        const char* name = provider->GetShaderName();

        ps->cs = renderDevice->CompileComputeShader(
            IRenderer::ShaderData{
                shaderData,dataSize,definesArray.data(),
                definesArray.size(), D3D_COMPILE_STANDARD_FILE_INCLUDE,
                nameShader,"cs_5_0",compileFlags,
#ifdef _DEBUG
                name
#endif
            }
        );

        computeDictinary.insert({id,ps});
        return {ps};
    }
}

PipelineFactory::~PipelineFactory()
{
    delete provider;
    for (auto& [key, ps] : dictinaryShaders)
    {
        renderDevice->AddDisposeVertexShader(ps->vs);
        renderDevice->AddDisposePixelShader(ps->ps);
        renderDevice->AddDisposeGeometryShader(ps->gs);
        renderDevice->AddDisposeComputeShader(ps->cs);
        delete ps;
    }
    free(shaderData);
    renderDevice->Flush();
}
