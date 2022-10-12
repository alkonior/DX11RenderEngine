#include "QuadRenderPass.h"

using namespace Renderer;


VertexBufferBinding QuadRenderPass<>::vertexBuffer;
Buffer* QuadRenderPass<>::indexBuffer = nullptr;
IRenderer* QuadRenderPass<>::renderDevice = nullptr;
bool QuadRenderPass<>::isInited = false;
size_t QuadRenderPass<>::renderPassesCounter = false;



