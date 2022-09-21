#pragma once
#include "IRenderer.h"
#include <map>
#include <vector>



namespace Renderer {

enum UsedShaders: uint16_t {
	UseGeometryShader = 1,
	UseComputeShader = 2,
};

class PipelineFactory {
	//IRenderer* renderer;
	IStateProvider* provider;


	void* shaderData;
	size_t dataSize;
	uint16_t compileFlags;

	const ShaderDefines* defines;
	size_t defineCount;

	uint32_t useShaders;

	std::map<size_t, PipelineState*> dictinary;
	std::map<std::pair<size_t, const char*>, PipelineState*> computeDictinary;

	PipelineFactory(IStateProvider* provider, const  ShaderDefines* defines, size_t defineCount, uint16_t compileFlags);
public:
	PipelineFactory(IStateProvider* provider, const  ShaderDefines* defines, size_t defineCount, void* shaderData, size_t dataSize, uint16_t compileFlags = 0);
	PipelineFactory(IStateProvider* provider, const  ShaderDefines* defines, size_t defineCount, void* shaderData, size_t dataSize, UsedShaders shaders, uint16_t compileFlags = 0);

	std::vector<ShaderDefines> GetDefines(size_t definesFlags);

	PipelineState* GetState(size_t definesFlags);
	PipelineState* GetComputeState(size_t definesFlags, const char* name);
	

	~PipelineFactory();
};

}

