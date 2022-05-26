#pragma once
#include "../CoreStructures/PipelineSnapshot.h"

namespace GVM
{
template <class T>
using VMStack = std::vector<T>;

typedef typename PipelineSnapshot::CompressedType PSC;

class VirtualMachine
{
    enum class EMachineCommands : uint8_t
    {
        DISPATCH,
        DISPATCH_INDIRECT,
        DRAW,
        DRAW_AUTO,
        DRAW_INDEXED,
        DRAW_INDEXED_INSTANCED,
        DRAW_INDEXED_INSTANCED_INDIRECT,
        DRAW_INSTANCED,
        DRAW_INSTANCED_INDIRECT,

        CREATE_RESOURCE,
        UPDATE_RESOURCE,
        SET_RESOURCE_DATA,
        COPY_RESOURCE_DATA,
        
        SET_VERTEX_BUFFER_DATA,
        SET_INDEX_BUFFER_DATA,
        SET_CONST_BUFFER_DATA,
    };
    IStructuresSize iStructSizes;

    VMStack<uint32_t> intStack;
    VMStack<EMachineCommands> commandStack;
    VMStack<uint8_t> dataStack;
    VMStack<uint8_t> pipelinesQueue;
    ResourcesManager resourcesManager;
    PSC* LastSnapshot = nullptr;

    //uint32_t PushData(void* data, uint32_t dataLength);


    IRenderDevice* RenderDevice;
public:
    
    VirtualMachine(const RenderDeviceInitParams& initParams, bool debugMode = true);
    VirtualMachine(const VirtualMachine&) = delete;
    VirtualMachine(const VirtualMachine&&) = delete;
    ~VirtualMachine() = default;


    void PushPSC(PipelineSnapshot& pipelineSnapshot);
    void PushDrawCall(DrawCall drawCall);


#pragma region Resources

    Resource* CreateResource(const ResourceDesc& desc);
    Resource* CreateBuffer(const BufferResourceDesc& desc);
    Resource* CreateTexture1D(const Texture1DResourceDesc& desc);
    Resource* CreateTexture2D(const Texture2DResourceDesc& desc);
    Resource* CreateTexture3D(const Texture3DResourceDesc& desc);
    Resource* CreateTextureCube(const TextureCubeResourceDesc& desc);
    VertexBuffer* CreateVertexBuffer(const BufferResourceDesc& desc);
    ConstBuffer* CreateConstBuffer(const BufferResourceDesc& desc);
    IndexBuffer* CreateIndexBuffer(const BufferResourceDesc& desc);

    ConstBufferView* CreateConstBufferView(const ConstBufferViewDesc& desc);
    VertexBufferView* CreateVertexBufferView(const VertexBufferViewDesc& desc);
    IndexBufferView* CreateIndexBufferView(const IndexBufferViewDesc& desc);
    DepthStencilView* CreateDepthStencilView(const DepthStencilViewDesc& desc);
    ShaderResourceView* CreateShaderResourceView(const ShaderResourceViewDesc& desc);
    RenderTargetView* CreateRenderTargetView(const RenderTargetViewDesc& desc);
    UATargetView* CreateUATargetView(const UATargetViewDesc& desc);

    void AddDisposeResource(const Resource* resource);
    void AddDisposeResourceView(const ResourceView* resourceView);

#pragma endregion



#pragma region Shaders

    //std::vector<IRenderDevice::IShader*> SavedShaders;
    //std::vector<IRenderDevice::IInputLayout*> SavedInputLayouts;

    Shader* CreateShader(const ShaderDesc& desc);
    InputLayout* CreateInputLayout(const InputAssemblerDeclarationDesc& desc);

#pragma endregion 

};
}
