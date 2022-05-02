#pragma once


namespace GVM {

class IRenderDeviceInitParams {};
class IViewport {};
class IScissorRect {};
class IBlendState {};
class IDepthStencilState {};
class IRasterizerState {};
class ISamplerState {};
class IPipelineState {};
class IInputLayout {};
class IResource {};
class IVertexBuffer : IResource {};
class IIndexBuffer : IResource {};
class IConstBuffer : IResource {};
class IResourceView {};
class IVertexBufferView : IResourceView {};
class IIndexBufferView : IResourceView {};
class IRenderTarget : IResource {};
class IRenderTargetView : IResourceView {};
class IShader {};

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
