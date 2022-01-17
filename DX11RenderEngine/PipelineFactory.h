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

	ShaderDefines* defines;
	size_t defineCount;

	std::map<size_t, PipelineState*> dictinary;
	PipelineFactory(IRenderer* renderer, IStateProvider* provider, ShaderDefines* defines, size_t defineCount);
public:
	PipelineFactory(IRenderer* renderer, IStateProvider* provider, ShaderDefines* defines, size_t defineCount, void* shaderData, size_t dataSize, void* inputLayout, size_t inputLayoutSize);

	std::vector<ShaderDefines>  GetDefines(size_t definesFlags);

	PipelineState* GetState(size_t definesFlags);
	

	~PipelineFactory();
};

}

