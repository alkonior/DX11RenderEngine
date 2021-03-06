#pragma once
#include "../../CoreStructures/PipelineSnapshot.h"

namespace GVM
{
template <class T>
using VMStack = std::vector<T>;

typedef typename PipelineSnapshot::CompressedType PSC;

class VirtualMachine
{
    enum class EMachineCommands : uint8_t
    {
        UNKNOWN,

        SETUP_PIPELINE,
        
        DRAW,

        CREATE_RESOURCE,
        CREATE_RESOURCE_VIEW,
        CREATE_SHADER,
        UPDATE_RESOURCE,
        SET_RESOURCE_DATA,
        COPY_RESOURCE_DATA,
    };
    
    constexpr static EMachineCommands ToCommand(EDrawCallType drawCall);
    IStructuresSize iStructSizes;

    VMStack<uint32_t> intStack;
    VMStack<EMachineCommands> commandQueue;
    VMStack<uint8_t> dataQueue;
    uint32_t drawCallsQueueShift = 0; 
    VMStack<DrawCall> drawCallsQueue;
    uint32_t pipelinesQueueShift = 0; 
    VMStack<uint8_t> pipelinesQueue;
    ResourcesManager resourcesManager;
    PSC* LastSnapshot = nullptr;

    //uint32_t PushData(void* data, uint32_t dataLength);

    void PushCommand(EMachineCommands command);

#pragma region Data
    void PushData(const void* data, uint32_t dataSize);
    
    template<class T>
    void PushData(const T& data)
    {
        constexpr uint32_t dataSize = sizeof(T);
        uint32_t position = dataQueue.size();
        dataQueue.resize(position+dataSize);
        memcpy_s(dataQueue.data()+position,dataSize,&data,dataSize);
    }
    
    template<class T>
    void PushData(T* data)
    {
        constexpr uint32_t dataSize = sizeof(*data);
        uint32_t position = dataQueue.size();
        dataQueue.resize(position+dataSize);
        memcpy_s(dataQueue.data()+position,dataSize,data,dataSize);
    }
    
    void PushData(void* data)
    {
        constexpr uint32_t dataSize = sizeof(uintptr_t);
        uint32_t position = dataQueue.size();
        dataQueue.resize(position+dataSize);
        *((void**)(dataQueue.data()+position)) = data;
    }
    
    uint32_t queueShift = 0;
    
    template<class T>
    const T& PullData()
    {
        constexpr uint32_t dataSize = sizeof(T);
        T* result = (T*)(dataQueue.data()+queueShift); 
        //uint32_t position = queueShift;
        //memcpy_s(dataQueue.data()+queueShift,dataSize,,dataSize);
        queueShift+=dataSize;
        return *(result);
    }
    
    void* PullPointer()
    {
        constexpr uint32_t dataSize = sizeof(uintptr_t);

        void* result = *((void**)(dataQueue.data()+queueShift));
        queueShift+=dataSize;
        return result;
    }
    
    const void* PullPointer(size_t dataLength)
    {
        void* result = dataQueue.data()+queueShift;
        queueShift+=dataLength;
        return result;
    }

#pragma endregion

#pragma Commands

void ExecuteSetupPipeline();
    
#pragma endregion

    IRenderDevice* RenderDevice = nullptr;
public:
    
    VirtualMachine(const RenderDeviceInitParams& initParams, bool debugMode = true);
    VirtualMachine(const VirtualMachine&) = delete;
    VirtualMachine(const VirtualMachine&&) = delete;
    ~VirtualMachine() = default;

    void Present();
    

    void PushPSC(const PipelineSnapshot& pipelineSnapshot);
    void PushDrawCall(const DrawCall& drawCall);


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
    DepthStencilView* CreateDepthStencilsView(const DepthStencilViewDesc& description);
    UATargetView* CreateUATargetView(const UATargetViewDesc& desc);

    void AddDisposeResource(const Resource* resource);
    void AddDisposeResourceView(const ResourceView* resourceView);

#pragma endregion

#pragma region Shaders

    //std::vector<IRenderDevice::IShader*> SavedShaders;
    //std::vector<IRenderDevice::IInputLayout*> SavedInputLayouts;

    Shader* CreateShader(const ShaderDesc& desc);
    InputLayout* CreateInputLayout(const InputAssemblerDeclarationDesc& desc, const ShaderDesc& Shader);

#pragma endregion

#pragma region SetResourcesData

    struct ResourceUpdateData
    {
        UBox rect = {};
        uint16_t dstSubresource = 0;
        int32_t srcRowPitch = 0;
        int32_t srcDepthPitch = 0;
        uint32_t dataSize = 0;
    };
    
    void SetResourceData(
        Resource* resource,
        uint16_t dstSubresource,
        const UBox& rect,
        const void* pSrcData,
        int32_t srcRowPitch = 0,
        int32_t srcDepthPitch = 0
    );

    void SetVertexBufferData(
        VertexBuffer* vertexBuffer,
        const void* pSrcData,
        uint32_t dataLength,
        uint32_t offset,
        int32_t srcRowPitch = 0,
        int32_t srcDepthPitch = 0
    );

    void SetIndexBufferData(
        IndexBuffer* buffer,
        const void* pSrcData,
        uint32_t dataLength,
        uint32_t offset,
        int32_t srcRowPitch = 0,
        int32_t srcDepthPitch = 0
    );
    
    void SetConstBufferData(
           ConstBuffer* constBuffer,
           const void* data,
           uint32_t dataSize,
           uint32_t offset);

#pragma endregion

};


}
