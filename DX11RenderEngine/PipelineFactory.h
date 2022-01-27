#pragma once
#include "Renderer.h"
#include "PipelineState.h"
#include <map>

namespace Renderer {

class PipelineFactory {
	IRenderer* renderer;
	IStateProvider* provider;

	void* shaderData;
	size_t dataSize;
	void* inputLayout;
	size_t inputLayoutSize;
	uint16_t compileFlags;

	ShaderDefines* defines;
	size_t defineCount;

	std::map<size_t, PipelineState*> dictinary;

	PipelineFactory(IRenderer* renderer, IStateProvider* provider, ShaderDefines* defines, size_t defineCount, uint16_t compileFlags);
public:
	PipelineFactory(IRenderer* renderer, IStateProvider* provider, ShaderDefines* defines, size_t defineCount, void* shaderData, size_t dataSize, void* inputLayout, size_t inputLayoutSize, uint16_t compileFlags = 0);

	std::vector<ShaderDefines>  GetDefines(size_t definesFlags);

	PipelineState* GetState(size_t definesFlags);
	

	~PipelineFactory();
};

}

