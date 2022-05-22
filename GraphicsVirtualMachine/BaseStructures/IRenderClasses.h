#pragma once
#include <cstdint>

namespace GVM {


struct Resource {};
struct ResourceView {};

struct Shader {};
struct InputLayout {};

struct VertexBuffer : Resource {};
struct IndexBuffer : Resource {};
struct ConstBuffer : Resource {};

struct VertexBufferView : ResourceView {};
struct IndexBufferView : ResourceView {};
struct ConstBufferView : ResourceView {};
struct RenderTargetView : ResourceView {};
struct UATargetView : ResourceView {};
struct DepthStencilView : ResourceView {};
struct ShaderResourceView : ResourceView {};

namespace Compressed {
    struct VertexBufferBinding {
        VertexBufferView* vertexBuffers [1];
        uint32_t vertexStride [1];
        uint32_t vertexOffset [1];
    };
}

struct VertexBufferBinding {
    using CompressedType = Compressed::VertexBufferBinding;
    uint8_t buffersNum = 0;
    VertexBufferView* vertexBuffers [32] = {};
    uint32_t vertexStride [32] = {};
    uint32_t vertexOffset [32] = {};
};

struct Mesh  {
    static const Mesh VoidMesh;
    VertexBufferBinding vertexBuffer;
    IndexBufferView* indexBuffer = nullptr;
};

}
