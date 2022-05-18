#pragma once
#include <cstdint>

namespace GVM {

struct IAbstract { virtual ~IAbstract() = 0 {}; };

class IInputLayout : IAbstract {};
class IResource : IAbstract {};
class IVertexBuffer : IResource {};
class IIndexBuffer : IResource {};
class IConstBuffer : IResource {};
class IResourceView : IAbstract {};
class IVertexBufferView : IResourceView {};
class IIndexBufferView : IResourceView {};
class IConstBufferView : IResourceView {};
class IRenderTarget : IResource {};
class IDepthStencil : IResource {};
class IRenderTargetView : IResourceView {};
class IDepthStencilView : IResourceView {};
class IShader : IAbstract {};

namespace Compressed {
    struct VertexBufferBinding {
        IVertexBufferView* vertexBuffers [1];
        uint32_t vertexStride [1];
        uint32_t vertexOffset [1];
    };
}

struct VertexBufferBinding {
    using CompressedType = Compressed::VertexBufferBinding;
    uint8_t buffersNum = 0;
    IVertexBufferView* vertexBuffers [32] = {};
    uint32_t vertexStride [32] = {};
    uint32_t vertexOffset [32] = {};
};

struct Mesh  {
    static const Mesh VoidMesh;
    VertexBufferBinding vertexBuffer;
    IIndexBufferView* indexBuffer = nullptr;
};

}
