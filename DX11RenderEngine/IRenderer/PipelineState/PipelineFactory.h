#pragma once
#include "IRenderer/IRenderer.h"
#include "PipelineState.h"
#include <map>



namespace Renderer {

enum UsedShaders: uint16_t {
	UseGeometryShader = 1,
	UseComputeShader = 2,
};

class PipelineFactory {
	IRenderer* renderer;
	IStateProvider* provider;


	void* shaderData;
	size_t dataSize;
	uint16_t compileFlags;

	const ShaderDefines* defines;
	size_t defineCount;

	uint32_t useShaders;

	std::map<size_t, PipelineState*> dictinary;

	PipelineFactory(IRenderer* renderer, IStateProvider* provider, const  ShaderDefines* defines, size_t defineCount, uint16_t compileFlags);
public:
	PipelineFactory(IRenderer* renderer, IStateProvider* provider, const  ShaderDefines* defines, size_t defineCount, void* shaderData, size_t dataSize, uint16_t compileFlags = 0);
	PipelineFactory(IRenderer* renderer, IStateProvider* provider, const  ShaderDefines* defines, size_t defineCount, void* shaderData, size_t dataSize, UsedShaders shaders, uint16_t compileFlags = 0);

	std::vector<ShaderDefines> GetDefines(size_t definesFlags);

	PipelineState* GetState(size_t definesFlags);
	

	~PipelineFactory();
};

}

