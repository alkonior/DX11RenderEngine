#pragma once


namespace GVM {

struct IAbstract { virtual ~IAbstract() = 0 {}; };

class IRenderDeviceInitParams : IAbstract {};
class IViewport : IAbstract {};
class IScissorRect : IAbstract {};
class IBlendState : IAbstract {};
class IDepthStencilState : IAbstract {};
class IRasterizerState : IAbstract {};
class ISamplerState : IAbstract {};
class IPipelineState : IAbstract {};
class IInputLayout : IAbstract {};
class IResource : IAbstract {};
class IVertexBuffer : IResource {};
class IIndexBuffer : IResource {};
class IConstBuffer : IResource {};
class IResourceView : IAbstract {};
class IVertexBufferView : IResourceView {};
class IIndexBufferView : IResourceView {};
class IRenderTarget : IResource {};
class IRenderTargetView : IResourceView {};
class IShader : IAbstract {};

struct VertexBufferBinding {
    uint8_t buffersCount = 0;
    IVertexBufferView* vertexBuffers [32];
    uint32_t vertexStride [32];
    uint32_t vertexOffset [32];
};

struct Mesh  {
    VertexBufferBinding vertexBuffer;
    IIndexBufferView* indexBuffer;
};

}
