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



}
