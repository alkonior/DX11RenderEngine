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
Renderer::IStateProvider* provider,
void* shaderData,
size_t dataSize):
    PipelineFactory(provider, provider->GetFactoryDescription().defines, provider->GetFactoryDescription().defineCount, provider->GetFactoryDescription().compileFlags)
{
    this->shaderData = malloc(dataSize);
    _memccpy(this->shaderData, shaderData, 1, dataSize);
    this->dataSize = dataSize;;
}


std::vector<ShaderDefines> PipelineFactory::GetDefines(size_t definesFlags)
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

PipelineState* PipelineFactory::GetState(size_t definesFlags)
{
    PipelineState* ps;
    if (dictinary.count(definesFlags /*int*/))
    {
        return dictinary[definesFlags];
    }
    {
        ps = new PipelineState();

        auto definesArray = GetDefines(definesFlags);
        const char* name = provider->GetShaderName();
        ps->ps = renderer->CompilePixelShader(
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

        auto inputDescriptor = provider->GetInputLayoutDescription(definesFlags);
        ps->vs = renderer->CompileVertexShader(
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
            ps->gs = renderer->CompileGeometryShader(
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
            ps->gs = nullptr;

        if (useShaders & Renderer::UseComputeShader)
            ps->cs = renderer->CompileComputeShader(
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
            ps->cs = nullptr;




        provider->PatchPipelineState(ps, definesFlags);

        dictinary.insert({definesFlags,ps});
        return ps;
    }
}

PipelineState* PipelineFactory::GetComputeState(size_t definesFlags, const char* nameShader)
{
    PipelineState* ps;
    auto id = std::make_pair(definesFlags, nameShader);
    if (computeDictinary.count(id))
    {
        return computeDictinary[id];
    }
    {
        ps = new PipelineState();

        auto definesArray = GetDefines(definesFlags);
        const char* name = provider->GetShaderName();

        ps->cs = renderer->CompileComputeShader(
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
        return ps;
    }
}

PipelineFactory::~PipelineFactory()
{
    delete provider;
    for (auto& [key, ps] : dictinary)
    {
        renderer->AddDisposeVertexShader(ps->vs);
        renderer->AddDisposePixelShader(ps->ps);
        delete ps;
    }
    free(shaderData);
    renderer->Flush();
}
