#include "PipelineFactory.h"
using namespace Renderer;

Renderer::PipelineFactory::PipelineFactory(IRenderer* renderer, IStateProvider* provider, ShaderDefines* defines, size_t defineCount):
	renderer(renderer),  provider(provider), defines(defines), defineCount(defineCount)
{}

Renderer::PipelineFactory::PipelineFactory(IRenderer* renderer, IStateProvider* provider, ShaderDefines* defines, size_t defineCount, void* shaderData, size_t dataSize, void* inputLayout, size_t inputLayoutSize):
	PipelineFactory(renderer, provider, defines, defineCount)
{
	this->dataSize = (dataSize); this->inputLayout = (inputLayout); this->inputLayoutSize = (inputLayoutSize);

	this->shaderData = malloc(dataSize);
	_memccpy(this->shaderData, shaderData, 1, dataSize);
	this->dataSize = dataSize;
}


std::vector<ShaderDefines> Renderer::PipelineFactory::GetDefines(size_t definesFlags) {
	std::vector<ShaderDefines> result;

	for (size_t i = 0; i < defineCount; i++) {
		if (definesFlags && 1) {
			result.push_back(ShaderDefines{ defines[i].defenition, defines[i].name });
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

		//D3DCompile(data, size, NULL, NULL, NULL, "psIn", "ps_4_0", NULL, NULL, &pPSData, &psErrorBlob);
		ps->ps = renderer->CompilePixelShader(shaderData, dataSize, definesArray.data(), definesArray.size(), NULL, "psIn", "ps_4_0", 0);

		//D3DCompile(data, size, NULL, NULL, NULL, "vsIn", "vs_4_0", NULL, NULL, &pPSData, &psErrorBlob);
		ps->vs = renderer->CompileVertexShader(shaderData, dataSize, definesArray.data(), definesArray.size(), NULL, "vsIn", "vs_4_0", 0, inputLayout, inputLayoutSize);

		//ps.vs = CompileShader(shaderPath, vs_5_0, "VSMain", get_defines, ...);
		//ps.ps = CompileShader(shaderPath, ps_5_0, "PSMain", get_defines, ...);

		//ps.cs = CompileShader(shaderPath, cs_5_0, "CSMain", get_defines, ...);


		provider->PatchPipelineState(ps);

		dictinary.insert({ definesFlags, ps });
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
}
