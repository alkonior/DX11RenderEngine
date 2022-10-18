#include "QuadRenderPass.h"

using namespace Renderer;

#define QUARPASSDEFINE(id) \
VertexBufferBinding QuadRenderPass<id>::vertexBuffer;\
Buffer* QuadRenderPass<id>::indexBuffer = nullptr;\
IRenderer* QuadRenderPass<id>::renderDevice = nullptr;\
bool QuadRenderPass<id>::isInited = false;\
size_t QuadRenderPass<id>::renderPassesCounter = false;


QUARPASSDEFINE(0)
QUARPASSDEFINE(1)
QUARPASSDEFINE(2)
QUARPASSDEFINE(3)
QUARPASSDEFINE(4)
QUARPASSDEFINE(5)
QUARPASSDEFINE(6)
QUARPASSDEFINE(7)
QUARPASSDEFINE(8)
QUARPASSDEFINE(9)
QUARPASSDEFINE(10)

