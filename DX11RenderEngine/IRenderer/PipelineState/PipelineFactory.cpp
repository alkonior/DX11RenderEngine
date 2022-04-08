#include "PipelineFactory.h"
using namespace Renderer;

Renderer::PipelineFactory::PipelineFactory(IStateProvider* provider,const ShaderDefines* defines, size_t defineCount, uint16_t compileFlags):
	provider(provider), defines(defines), defineCount(defineCount), compileFlags(compileFlags), dataSize(0), shaderData(nullptr)
{}

Renderer::PipelineFactory::PipelineFactory(IStateProvider* provider,const  ShaderDefines* defines, size_t defineCount, void* shaderData, size_t dataSize, uint16_t compileFlags):
	PipelineFactory(provider, defines, defineCount, compileFlags)
{
	this->dataSize = (dataSize); 
	this->shaderData = malloc(dataSize);
	_memccpy(this->shaderData, shaderData, 1, dataSize);
	this->dataSize = dataSize;
}

Renderer::PipelineFactory::PipelineFactory(IStateProvider* provider, const ShaderDefines* defines, size_t defineCount, void* shaderData, size_t dataSize, UsedShaders shaders, uint16_t compileFlags) :
	PipelineFactory(provider, defines, defineCount,  shaderData, dataSize, compileFlags)
{
	useShaders = shaders;
}


std::vector<ShaderDefines> Renderer::PipelineFactory::GetDefines(size_t definesFlags) {
	std::vector<ShaderDefines> result;

	for (size_t i = 0; i < defineCount; i++) {
		if (definesFlags & size_t(1)<<i) {
			result.push_back(ShaderDefines{ defines[i].name ,defines[i].defenition });
		}
	}

	result.shrink_to_fit();

	return result;
}

PipelineState* Renderer::PipelineFactory::GetState(size_t definesFlags) {
	PipelineState* ps;
	if (dictinary.count(definesFlags /*int*/)) {
		return dictinary[definesFlags];
	}
	{
		ps = new PipelineState();
			
		auto definesArray = GetDefines(definesFlags);
		const char* name = provider->GetShaderName(); 
		ps->ps = renderer->CompilePixelShader(
			IRenderer::ShaderData{
			shaderData, dataSize,
			definesArray.data(),
			definesArray.size(),
			D3D_COMPILE_STANDARD_FILE_INCLUDE, 
			"psIn", "ps_4_0", compileFlags,
#ifdef _DEBUG
				name
#endif
			}
			);

		auto inputDescriptor = provider->GetInputLayoutDescription(definesFlags);
		ps->vs = renderer->CompileVertexShader(
			IRenderer::ShaderData{
			shaderData, dataSize, definesArray.data(),
			definesArray.size(), D3D_COMPILE_STANDARD_FILE_INCLUDE,
			"vsIn", "vs_4_0", compileFlags, 
#ifdef _DEBUG
				name
#endif
			},inputDescriptor.inputLayout, inputDescriptor.inputLayoutSize
			);

		if (useShaders & UseGeometryShader) 
			ps->gs = renderer->CompileGeometryShader(
			IRenderer::ShaderData{
			shaderData, dataSize, definesArray.data(),
				definesArray.size(), D3D_COMPILE_STANDARD_FILE_INCLUDE,
				"gsIn", "gs_4_0", compileFlags,
#ifdef _DEBUG
				name
#endif
				}
				);
		else
			ps->gs = nullptr;

		if (useShaders & UseComputeShader) 
			ps->cs = renderer->CompileComputeShader(
			IRenderer::ShaderData{
			shaderData, dataSize, definesArray.data(),
				definesArray.size(), D3D_COMPILE_STANDARD_FILE_INCLUDE,
				"csIn", "cs_4_0", compileFlags,
#ifdef _DEBUG
				name
#endif
				}
				);
		else
			ps->cs = nullptr;




		provider->PatchPipelineState(ps, definesFlags);

		dictinary.insert({ definesFlags, ps });
		return ps;
	}
}

PipelineState* Renderer::PipelineFactory::GetComputeState(size_t definesFlags, const char* nameShader) {
	PipelineState* ps;
	auto id = std::make_pair(definesFlags, nameShader);
	if (computeDictinary.count(id )) {
		return computeDictinary[id];
	}
	{
		ps = new PipelineState();
			
		auto definesArray = GetDefines(definesFlags);
		const char* name = provider->GetShaderName();
		
		ps->cs = renderer->CompileComputeShader(
		IRenderer::ShaderData{
		shaderData, dataSize, definesArray.data(),
				definesArray.size(), D3D_COMPILE_STANDARD_FILE_INCLUDE,
				nameShader, "cs_5_0", compileFlags,
#ifdef _DEBUG
				name
#endif
			}
			);

		computeDictinary.insert({ id, ps });
		return ps;
	}
}

Renderer::PipelineFactory::~PipelineFactory() {
	delete provider;
	for (auto& [key, ps] : dictinary) {
		renderer->AddDisposeVertexShader(ps->vs);
		renderer->AddDisposePixelShader(ps->ps);
		delete ps;
	}
	free(shaderData);
	renderer->Flush();
}
