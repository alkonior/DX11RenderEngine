#pragma once
#include "CoreStructures\PipelineSnapshot.h"

namespace GVM
{
    template <class T>
    using VMStack = std::vector<T>;

    typedef typename Compressed::PipelineSnapshot PSC;

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


        VMStack<uint32_t> intStack;
        VMStack<EMachineCommands> commandStack;
        VMStack<uint8_t> dataStack;
        VMStack<uint8_t> pipelinesQueue;
        ResourcesManager resourcesManager;

        uint32_t PushData(void* data, uint32_t dataLength);
    public:
        VirtualMachine(const RenderDeviceInitParams& initParams, bool debugMode = true);
        VirtualMachine(const VirtualMachine&) = delete;
        VirtualMachine(const VirtualMachine&&) = delete;
        ~VirtualMachine() = default;


        void PushPSC(PipelineSnapshot& pipelineSnapshot);
        void PushDrawCall(DrawCall drawCall);


#pragma region Resources

        const Resource* CreateResource(const ResourceDesc& desc);
        const Resource* CreateBuffer(const BufferResourceDesc& desc);
        const Resource* CreateTexture1D(const Texture1DResourceDesc& desc);
        const Resource* CreateTexture2D(const Texture2DResourceDesc& desc);
        const Resource* CreateTexture3D(const Texture3DResourceDesc& desc);
        const Resource* CreateTextureCube(const TextureCubeResourceDesc& desc);
        const VertexBuffer* CreateVertexBuffer(const BufferResourceDesc& desc);
        const ConstBuffer* CreateConstBuffer(const BufferResourceDesc& desc);
        const IndexBuffer* CreateIndexBuffer(const BufferResourceDesc& desc);

        const ConstBufferView* CreateConstBufferView(const ConstBufferViewDesc& desc);
        const VertexBufferView* CreateVertexBufferView(const VertexBufferViewDesc& desc);
        const IndexBufferView* CreateIndexBufferView(const IndexBufferViewDesc& desc);
        const DepthStencilView* CreateDepthStencilView(const DepthStencilViewDesc& desc);
        const ShaderResourceView* CreateShaderResourceView(const ShaderResourceViewDesc& desc);
        const RenderTargetView* CreateRenderTargetView(const RenderTargetViewDesc& desc);
        const UATargetView* CreateUATargetView(const UATargetViewDesc& desc);

        void AddDisposeResource(const Resource* resource);
        void AddDisposeResourceView(const ResourceView* resourceView);

#pragma endregion




    };
}
