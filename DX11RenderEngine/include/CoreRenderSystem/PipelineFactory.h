#pragma once
#include "IRenderer.h"
//#include "RenderFlags.h"
#include <map>
#include <vector>

enum UsedShaders: uint16_t {
	UseGeometryShader = 1,
	UseComputeShader = 2,
};

class PipelineFactory {
	
	Renderer::IRenderer* renderDevice;
	Renderer::IStateProvider* provider;
	
	void* shaderData;
	size_t dataSize;
	uint16_t compileFlags;

	const Renderer::ShaderDefines* defines;
	size_t defineCount;

	uint32_t useShaders;

	std::map<size_t, Renderer::PipelineState*> dictinary;
	std::map<std::pair<size_t, const char*>, Renderer::PipelineState*> computeDictinary;



	PipelineFactory(Renderer::IStateProvider* provider, const  Renderer::ShaderDefines* defines, size_t defineCount, uint16_t compileFlags);
public:
	PipelineFactory(
		Renderer::IRenderer* renderDevice,
		Renderer::IStateProvider* provider,
		void* shaderData,
		size_t dataSize);

	std::vector<Renderer::ShaderDefines> GetDefines(size_t definesFlags);

	Renderer::PipelineState* GetState(size_t definesFlags);
	Renderer::PipelineState* GetComputeState(size_t definesFlags, const char* name);
	

	~PipelineFactory();
};


