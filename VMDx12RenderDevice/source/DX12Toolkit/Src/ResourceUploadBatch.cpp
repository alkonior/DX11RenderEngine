//--------------------------------------------------------------------------------------
// File: ResourceUploadBatch.cpp
//
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkID=615561
//--------------------------------------------------------------------------------------

#include "pch.h"
#include "ResourceUploadBatch.h"

#include "DirectXHelpers.h"
#include "PlatformHelpers.h"

using namespace DirectX;
using Microsoft::WRL::ComPtr;

#ifdef __MINGW32__
const GUID IID_ID3D12Device = { 0x189819f1, 0x1db6, 0x4b57, { 0xbe, 0x54, 0x18, 0x21, 0x33, 0x9b, 0x85, 0xf7 } };
#endif

// Include the precompiled shader code.
namespace
{
#ifdef _GAMING_XBOX_SCARLETT
#include "XboxGamingScarlettGenerateMips_main.inc"
#elif defined(_GAMING_XBOX)
#include "XboxGamingXboxOneGenerateMips_main.inc"
#elif defined(_XBOX_ONE) && defined(_TITLE)
#include "XboxOneGenerateMips_main.inc"
#else

#endif

    bool FormatIsUAVCompatible(_In_ ID3D12Device* device, bool typedUAVLoadAdditionalFormats, DXGI_FORMAT format) noexcept
    {
        switch (format)
        {
        case DXGI_FORMAT_R32_FLOAT:
        case DXGI_FORMAT_R32_UINT:
        case DXGI_FORMAT_R32_SINT:
            // Unconditionally supported.
            return true;

        case DXGI_FORMAT_R32G32B32A32_FLOAT:
        case DXGI_FORMAT_R32G32B32A32_UINT:
        case DXGI_FORMAT_R32G32B32A32_SINT:
        case DXGI_FORMAT_R16G16B16A16_FLOAT:
        case DXGI_FORMAT_R16G16B16A16_UINT:
        case DXGI_FORMAT_R16G16B16A16_SINT:
        case DXGI_FORMAT_R8G8B8A8_UNORM:
        case DXGI_FORMAT_R8G8B8A8_UINT:
        case DXGI_FORMAT_R8G8B8A8_SINT:
        case DXGI_FORMAT_R16_FLOAT:
        case DXGI_FORMAT_R16_UINT:
        case DXGI_FORMAT_R16_SINT:
        case DXGI_FORMAT_R8_UNORM:
        case DXGI_FORMAT_R8_UINT:
        case DXGI_FORMAT_R8_SINT:
            // All these are supported if this optional feature is set.
            return typedUAVLoadAdditionalFormats;

        case DXGI_FORMAT_R16G16B16A16_UNORM:
        case DXGI_FORMAT_R16G16B16A16_SNORM:
        case DXGI_FORMAT_R32G32_FLOAT:
        case DXGI_FORMAT_R32G32_UINT:
        case DXGI_FORMAT_R32G32_SINT:
        case DXGI_FORMAT_R10G10B10A2_UNORM:
        case DXGI_FORMAT_R10G10B10A2_UINT:
        case DXGI_FORMAT_R11G11B10_FLOAT:
        case DXGI_FORMAT_R8G8B8A8_SNORM:
        case DXGI_FORMAT_R16G16_FLOAT:
        case DXGI_FORMAT_R16G16_UNORM:
        case DXGI_FORMAT_R16G16_UINT:
        case DXGI_FORMAT_R16G16_SNORM:
        case DXGI_FORMAT_R16G16_SINT:
        case DXGI_FORMAT_R8G8_UNORM:
        case DXGI_FORMAT_R8G8_UINT:
        case DXGI_FORMAT_R8G8_SNORM:
        case DXGI_FORMAT_R8G8_SINT:
        case DXGI_FORMAT_R16_UNORM:
        case DXGI_FORMAT_R16_SNORM:
        case DXGI_FORMAT_R8_SNORM:
        case DXGI_FORMAT_A8_UNORM:
        case DXGI_FORMAT_B5G6R5_UNORM:
        case DXGI_FORMAT_B5G5R5A1_UNORM:
        case DXGI_FORMAT_B4G4R4A4_UNORM:
            // Conditionally supported by specific devices.
            if (typedUAVLoadAdditionalFormats)
            {
                D3D12_FEATURE_DATA_FORMAT_SUPPORT formatSupport = { format, D3D12_FORMAT_SUPPORT1_NONE, D3D12_FORMAT_SUPPORT2_NONE };
                if (SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &formatSupport, sizeof(formatSupport))))
                {
                    const DWORD mask = D3D12_FORMAT_SUPPORT2_UAV_TYPED_LOAD | D3D12_FORMAT_SUPPORT2_UAV_TYPED_STORE;
                    return ((formatSupport.Support2 & mask) == mask);
                }
            }
            return false;

        default:
            return false;
        }
    }

    bool FormatIsBGR(DXGI_FORMAT format) noexcept
    {
        switch (format)
        {
        case DXGI_FORMAT_B8G8R8A8_UNORM:
        case DXGI_FORMAT_B8G8R8X8_UNORM:
        case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
        case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
            return true;
        default:
            return false;
        }
    }

    bool FormatIsSRGB(DXGI_FORMAT format) noexcept
    {
        switch (format)
        {
        case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
        case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
        case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
            return true;
        default:
            return false;
        }
    }

    DXGI_FORMAT ConvertSRVtoResourceFormat(DXGI_FORMAT format) noexcept
    {
        switch (format)
        {
        case DXGI_FORMAT_R32G32B32A32_FLOAT:
        case DXGI_FORMAT_R32G32B32A32_UINT:
        case DXGI_FORMAT_R32G32B32A32_SINT:
            return DXGI_FORMAT_R32G32B32A32_TYPELESS;

        case DXGI_FORMAT_R16G16B16A16_FLOAT:
        case DXGI_FORMAT_R16G16B16A16_UNORM:
        case DXGI_FORMAT_R16G16B16A16_UINT:
        case DXGI_FORMAT_R16G16B16A16_SNORM:
        case DXGI_FORMAT_R16G16B16A16_SINT:
            return DXGI_FORMAT_R16G16B16A16_TYPELESS;

        case DXGI_FORMAT_R32G32_FLOAT:
        case DXGI_FORMAT_R32G32_UINT:
        case DXGI_FORMAT_R32G32_SINT:
            return DXGI_FORMAT_R32G32_TYPELESS;

        case DXGI_FORMAT_R10G10B10A2_UNORM:
        case DXGI_FORMAT_R10G10B10A2_UINT:
            return DXGI_FORMAT_R10G10B10A2_TYPELESS;

        case DXGI_FORMAT_R8G8B8A8_UNORM:
        case DXGI_FORMAT_R8G8B8A8_UINT:
        case DXGI_FORMAT_R8G8B8A8_SNORM:
        case DXGI_FORMAT_R8G8B8A8_SINT:
            return DXGI_FORMAT_R8G8B8A8_TYPELESS;

        case DXGI_FORMAT_R16G16_FLOAT:
        case DXGI_FORMAT_R16G16_UNORM:
        case DXGI_FORMAT_R16G16_UINT:
        case DXGI_FORMAT_R16G16_SNORM:
        case DXGI_FORMAT_R16G16_SINT:
            return DXGI_FORMAT_R16G16_TYPELESS;

        case DXGI_FORMAT_R32_FLOAT:
        case DXGI_FORMAT_R32_UINT:
        case DXGI_FORMAT_R32_SINT:
            return DXGI_FORMAT_R32_TYPELESS;

        case DXGI_FORMAT_R8G8_UNORM:
        case DXGI_FORMAT_R8G8_UINT:
        case DXGI_FORMAT_R8G8_SNORM:
        case DXGI_FORMAT_R8G8_SINT:
            return DXGI_FORMAT_R8G8_TYPELESS;

        case DXGI_FORMAT_R16_FLOAT:
        case DXGI_FORMAT_R16_UNORM:
        case DXGI_FORMAT_R16_UINT:
        case DXGI_FORMAT_R16_SNORM:
        case DXGI_FORMAT_R16_SINT:
            return DXGI_FORMAT_R16_TYPELESS;

        case DXGI_FORMAT_R8_UNORM:
        case DXGI_FORMAT_R8_UINT:
        case DXGI_FORMAT_R8_SNORM:
        case DXGI_FORMAT_R8_SINT:
            return DXGI_FORMAT_R8_TYPELESS;

        default:
            return format;
        }
    }
} // anonymous namespace

class ResourceUploadBatch::Impl
{
public:
    Impl(
        _In_ ID3D12Device* device) noexcept
        : mDevice(device)
        , mCommandType(D3D12_COMMAND_LIST_TYPE_DIRECT)
        , mInBeginEndBlock(false)
        , mTypedUAVLoadAdditionalFormats(false)
        , mStandardSwizzle64KBSupported(false)
    {
        assert(device != nullptr);
        D3D12_FEATURE_DATA_D3D12_OPTIONS options = {};
        if (SUCCEEDED(device->CheckFeatureSupport(
            D3D12_FEATURE_D3D12_OPTIONS,
            &options,
            sizeof(options))))
        {
            mTypedUAVLoadAdditionalFormats = options.TypedUAVLoadAdditionalFormats != 0;
            mStandardSwizzle64KBSupported = options.StandardSwizzle64KBSupported != 0;
        }
    }

    // Call this before your multiple calls to Upload.
    void Begin(D3D12_COMMAND_LIST_TYPE commandType)
    {
        if (mInBeginEndBlock)
            throw std::logic_error("Can't Begin: already in a Begin-End block.");

        switch (commandType)
        {
        case D3D12_COMMAND_LIST_TYPE_DIRECT:
        case D3D12_COMMAND_LIST_TYPE_COMPUTE:
        case D3D12_COMMAND_LIST_TYPE_COPY:
            break;

        default:
            DebugTrace("ResourceUploadBatch only supports Direct, Compute, and Copy command queues\n");
            throw std::invalid_argument("commandType parameter is invalid");
        }

        ThrowIfFailed(mDevice->CreateCommandAllocator(commandType, IID_GRAPHICS_PPV_ARGS(mCmdAlloc.ReleaseAndGetAddressOf())));

        SetDebugObjectName(mCmdAlloc.Get(), L"ResourceUploadBatch");

        ThrowIfFailed(mDevice->CreateCommandList(1, commandType, mCmdAlloc.Get(), nullptr, IID_GRAPHICS_PPV_ARGS(mList.ReleaseAndGetAddressOf())));

        SetDebugObjectName(mList.Get(), L"ResourceUploadBatch");

        mCommandType = commandType;
        mInBeginEndBlock = true;
    }

    // Asynchronously uploads a resource. The memory in subRes is copied.
    // The resource must be in the COPY_DEST state.
    void Upload(
        _In_ ID3D12Resource* resource,
        uint32_t subresourceIndexStart,
        _In_reads_(numSubresources) D3D12_SUBRESOURCE_DATA* subRes,
        uint32_t numSubresources)
    {
        if (!mInBeginEndBlock)
            throw std::logic_error("Can't call Upload on a closed ResourceUploadBatch.");

        const UINT64 uploadSize = GetRequiredIntermediateSize(
            resource,
            subresourceIndexStart,
            numSubresources);

        const CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
        auto const resDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadSize);

        // Create a temporary buffer
        ComPtr<ID3D12Resource> scratchResource = nullptr;
        ThrowIfFailed(mDevice->CreateCommittedResource(
            &heapProps,
            D3D12_HEAP_FLAG_NONE,
            &resDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr, // D3D12_CLEAR_VALUE* pOptimizedClearValue
            IID_GRAPHICS_PPV_ARGS(scratchResource.GetAddressOf())));

        SetDebugObjectName(scratchResource.Get(), L"ResourceUploadBatch Temporary");

        // Submit resource copy to command list
        UpdateSubresources(
            mList.Get(),
            resource,
            scratchResource.Get(),
            0,
            subresourceIndexStart,
            numSubresources,
            subRes
        );

        // Remember this upload object for delayed release
        mTrackedObjects.push_back(scratchResource);
    }

    void Upload(
        _In_ ID3D12Resource* resource,
        const SharedGraphicsResource& buffer)
    {
        if (!mInBeginEndBlock)
            throw std::logic_error("Can't call Upload on a closed ResourceUploadBatch.");

        // Submit resource copy to command list
        mList->CopyBufferRegion(resource, 0, buffer.Resource(), buffer.ResourceOffset(), buffer.Size());

        // Remember this upload resource for delayed release
        mTrackedMemoryResources.push_back(buffer);
    }


    // Transition a resource once you're done with it
    void Transition(
        _In_ ID3D12Resource* resource,
        _In_ D3D12_RESOURCE_STATES stateBefore,
        _In_ D3D12_RESOURCE_STATES stateAfter)
    {
        if (!mInBeginEndBlock)
            throw std::logic_error("Can't call Upload on a closed ResourceUploadBatch.");

        if (mCommandType == D3D12_COMMAND_LIST_TYPE_COPY)
        {
            switch (stateAfter)
            {
            case D3D12_RESOURCE_STATE_COPY_DEST:
            case D3D12_RESOURCE_STATE_COPY_SOURCE:
                break;

            default:
                // Ignore other states for copy queues.
                return;
            }
        }
        else if (mCommandType == D3D12_COMMAND_LIST_TYPE_COMPUTE)
        {
            switch (stateAfter)
            {
            case D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER:
            case D3D12_RESOURCE_STATE_UNORDERED_ACCESS:
            case D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE:
            case D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT:
            case D3D12_RESOURCE_STATE_COPY_DEST:
            case D3D12_RESOURCE_STATE_COPY_SOURCE:
                break;

            default:
                // Ignore other states for compute queues.
                return;
            }
        }

        TransitionResource(mList.Get(), resource, stateBefore, stateAfter);
    }

    // Submits all the uploads to the driver.
    // No more uploads can happen after this call until Begin is called again.
    // This returns a handle to an event that can be waited on.
    std::future<void> End(
        _In_ ID3D12CommandQueue* commandQueue)
    {
        if (!mInBeginEndBlock)
            throw std::logic_error("ResourceUploadBatch already closed.");

        ThrowIfFailed(mList->Close());

        // Submit the job to the GPU
        commandQueue->ExecuteCommandLists(1, CommandListCast(mList.GetAddressOf()));

        // Set an event so we get notified when the GPU has completed all its work
        ComPtr<ID3D12Fence> fence;
        ThrowIfFailed(mDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_GRAPHICS_PPV_ARGS(fence.GetAddressOf())));

        SetDebugObjectName(fence.Get(), L"ResourceUploadBatch");

        HANDLE gpuCompletedEvent = CreateEventEx(nullptr, nullptr, 0, EVENT_MODIFY_STATE | SYNCHRONIZE);
        if (!gpuCompletedEvent)
            throw std::system_error(std::error_code(static_cast<int>(GetLastError()), std::system_category()), "CreateEventEx");

        ThrowIfFailed(commandQueue->Signal(fence.Get(), 1ULL));
        ThrowIfFailed(fence->SetEventOnCompletion(1ULL, gpuCompletedEvent));

        // Create a packet of data that'll be passed to our waiting upload thread
        auto uploadBatch = new UploadBatch();
        uploadBatch->CommandList = mList;
        uploadBatch->Fence = fence;
        uploadBatch->GpuCompleteEvent.reset(gpuCompletedEvent);
        std::swap(mTrackedObjects, uploadBatch->TrackedObjects);
        std::swap(mTrackedMemoryResources, uploadBatch->TrackedMemoryResources);

        // Kick off a thread that waits for the upload to complete on the GPU timeline.
        // Let the thread run autonomously, but provide a future the user can wait on.
        std::future<void> future = std::async(std::launch::async, [uploadBatch]()
            {
                // Wait on the GPU-complete notification
                const DWORD wr = WaitForSingleObject(uploadBatch->GpuCompleteEvent.get(), INFINITE);
                if (wr != WAIT_OBJECT_0)
                {
                    if (wr == WAIT_FAILED)
                    {
                        throw std::system_error(std::error_code(static_cast<int>(GetLastError()), std::system_category()), "WaitForSingleObject");
                    }
                    else
                    {
                        throw std::runtime_error("WaitForSingleObject");
                    }
                }

                // Delete the batch
                // Because the vectors contain smart-pointers, their destructors will
                // fire and the resources will be released.
                delete uploadBatch;
            });

            // Reset our state
        mCommandType = D3D12_COMMAND_LIST_TYPE_DIRECT;
        mInBeginEndBlock = false;
        mList.Reset();
        mCmdAlloc.Reset();

        // Swap above should have cleared these
        assert(mTrackedObjects.empty());
        assert(mTrackedMemoryResources.empty());

        return future;
    }

    bool IsSupportedForGenerateMips(DXGI_FORMAT format) noexcept
    {
        if (mCommandType == D3D12_COMMAND_LIST_TYPE_COPY)
            return false;

        if (FormatIsUAVCompatible(mDevice.Get(), mTypedUAVLoadAdditionalFormats, format))
            return true;

        if (FormatIsBGR(format))
        {
        #if defined(_GAMING_XBOX) || (defined(_XBOX_ONE) && defined(_TITLE))
                    // We know the RGB and BGR memory layouts match for Xbox One
            return true;
        #else
                    // BGR path requires DXGI_FORMAT_R8G8B8A8_UNORM support for UAV load/store plus matching layouts
            return mTypedUAVLoadAdditionalFormats && mStandardSwizzle64KBSupported;
        #endif
        }

        if (FormatIsSRGB(format))
        {
            // sRGB path requires DXGI_FORMAT_R8G8B8A8_UNORM support for UAV load/store
            return mTypedUAVLoadAdditionalFormats;
        }

        return false;
    }

private:

    struct UploadBatch
    {
        std::vector<ComPtr<ID3D12DeviceChild>>  TrackedObjects;
        std::vector<SharedGraphicsResource>     TrackedMemoryResources;
        ComPtr<ID3D12GraphicsCommandList>       CommandList;
        ComPtr<ID3D12Fence>                     Fence;
        ScopedHandle                            GpuCompleteEvent;

        UploadBatch() noexcept {}
    };

    ComPtr<ID3D12Device>                        mDevice;
    ComPtr<ID3D12CommandAllocator>              mCmdAlloc;
    ComPtr<ID3D12GraphicsCommandList>           mList;

    std::vector<ComPtr<ID3D12DeviceChild>>      mTrackedObjects;
    std::vector<SharedGraphicsResource>         mTrackedMemoryResources;

    D3D12_COMMAND_LIST_TYPE                     mCommandType;
    bool                                        mInBeginEndBlock;
    bool                                        mTypedUAVLoadAdditionalFormats;
    bool                                        mStandardSwizzle64KBSupported;
};



// Public constructor.
ResourceUploadBatch::ResourceUploadBatch(_In_ ID3D12Device* device) noexcept(false)
    : pImpl(std::make_unique<Impl>(device))
{
}


ResourceUploadBatch::ResourceUploadBatch(ResourceUploadBatch&&) noexcept = default;
ResourceUploadBatch& ResourceUploadBatch::operator= (ResourceUploadBatch&&) noexcept = default;
ResourceUploadBatch::~ResourceUploadBatch() = default;


void ResourceUploadBatch::Begin(D3D12_COMMAND_LIST_TYPE commandType)
{
    pImpl->Begin(commandType);
}


_Use_decl_annotations_
void ResourceUploadBatch::Upload(
    ID3D12Resource* resource,
    uint32_t subresourceIndexStart,
    D3D12_SUBRESOURCE_DATA* subRes,
    uint32_t numSubresources)
{
    pImpl->Upload(resource, subresourceIndexStart, subRes, numSubresources);
}


_Use_decl_annotations_
void ResourceUploadBatch::Upload(
    ID3D12Resource* resource,
    const SharedGraphicsResource& buffer
)
{
    pImpl->Upload(resource, buffer);
}



_Use_decl_annotations_
void ResourceUploadBatch::Transition(
    ID3D12Resource* resource,
    D3D12_RESOURCE_STATES stateBefore,
    D3D12_RESOURCE_STATES stateAfter)
{
    pImpl->Transition(resource, stateBefore, stateAfter);
}


std::future<void> ResourceUploadBatch::End(_In_ ID3D12CommandQueue* commandQueue)
{
    return pImpl->End(commandQueue);
}


bool __cdecl ResourceUploadBatch::IsSupportedForGenerateMips(DXGI_FORMAT format) noexcept
{
    return pImpl->IsSupportedForGenerateMips(format);
}
