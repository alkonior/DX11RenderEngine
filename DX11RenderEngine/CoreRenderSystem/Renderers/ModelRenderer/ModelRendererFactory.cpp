#include "ModelRendererFactory.h"


using namespace Renderer;



ModelRendererFactory::ModelRendererFactory(IRenderer* renderer, IStateProvider* provider, void* shaderData, size_t dataSize)
    :PipelineFactory(renderer, provider, (const ShaderDefines*)ModelRendererDefines, std::size(ModelRendererDefines), shaderData, dataSize, D3DCOMPILE_DEBUG) {
}