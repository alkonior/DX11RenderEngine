#pragma once
#include "IRenderer/Utils.h"
#include "IRenderer/BlendState.h"
#include "IRenderer/DepthStencilState.h"
#include "IRenderer/RasterizerState.h"
#include "IRenderer/SamplerState.h"

namespace Renderer {

struct PipelineShaders {
	PixelShader* ps = nullptr;
	VertexShader* vs = nullptr;

	GeometryShader* gs = nullptr;
	ComputeShader* cs = nullptr;
};

struct Pipeline {
	DepthStencilState* dss = nullptr;
	BlendState* bs = nullptr;
	RasterizerState* rs = nullptr; 
};

struct PipelineState {
	PipelineShaders* shaders = nullptr;
	Pipeline* pipeline = nullptr;
	
};

enum UsedShaders: uint16_t {
	UsePSVS = 0,
	UseGeometryShader = 1,
	UseComputeShader = 2,
};

struct InputLayoutDescription {
	void* inputLayout = nullptr;
	size_t inputLayoutSize = 0;
};

struct PipelineFactoryDescription {
	const  Renderer::ShaderDefines* defines = nullptr;
	size_t defineCount = 0;
	uint16_t compileFlags
#if _DEBUG
	=1;
#else
	= 0;
#endif
};
	
struct ShaderCompileData
{
	void* shaderData;
	size_t dataSize;
	ShaderDefines* defines;
	size_t definesSize;
	void* includes;
	const char* enteryPoint;
	const char* target;
	uint16_t flags;
#ifdef _DEBUG
	const char* name;
#endif
};

struct IStateProvider {
	virtual void PatchPipelineState(Pipeline* refToPS, uint32_t definesFlags) = 0;
	virtual InputLayoutDescription GetInputLayoutDescription(uint32_t definesFlags) = 0;
	virtual PipelineFactoryDescription GetFactoryDescription() = 0;
	virtual const char* GetShaderName() = 0;
	virtual ~IStateProvider() = default;
};

}
