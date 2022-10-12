#pragma once
#include "IRenderer/Utils.h"
#include "IRenderer/BlendState.h"
#include "IRenderer/DepthStencilState.h"
#include "IRenderer/RasterizerState.h"
#include "IRenderer/SamplerState.h"

namespace Renderer {
struct PipelineState {


	PixelShader* ps = nullptr;
	VertexShader* vs = nullptr;

	GeometryShader* gs = nullptr;
	ComputeShader* cs = nullptr;

	DepthStencilState* dss = nullptr;
	BlendState* bs = nullptr;
	RasterizerState* rs = nullptr; 

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
	UsedShaders shaders = (UsedShaders)0;
	uint16_t compileFlags
#if _DEBUG
	=1;
#else
	= 0;
#endif
};
	


struct IStateProvider {
	virtual void PatchPipelineState(PipelineState* refToPS, size_t definesFlags) = 0;
	virtual InputLayoutDescription GetInputLayoutDescription(size_t definesFlags) = 0;
	virtual PipelineFactoryDescription GetFactoryDescription() = 0;
	virtual const char* GetShaderName() = 0;
	virtual ~IStateProvider() = default;
};

}
