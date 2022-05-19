#include "ResourcesManager.h"

using namespace GVM;

uint32_t ResourcesManager::CalculateResourceSize(const BufferResourceDesc& desc)
{
    return desc.Size;
}

uint32_t ResourcesManager::CalculateResourceSize(const TextureResourceDesc& desc)
{
    uint32_t size;
    uint8_t numLevels = desc.MipLevels;
    if (!numLevels)
        numLevels = 1 + floor(log2(std::max(desc.Height, desc.Height, desc.DepthOrArraySize)));
    uint32_t width = desc.Height;
    uint32_t height = desc.Height;
    uint32_t depth = desc.Height;
    
    for (numLevels)
    {
        
    }
}

uint32_t ResourcesManager::CalculateResourceSize(const Texture1DResourceDesc& desc)
{
}

uint32_t ResourcesManager::CalculateResourceSize(const Texture2DResourceDesc& desc)
{
}

uint32_t ResourcesManager::CalculateResourceSize(const Texture3DResourceDesc& desc)
{
}

uint32_t ResourcesManager::CalculateResourceSize(const TextureCubeResourceDesc& desc)
{
}

ResourcesManager::ResourcesManager(RenderDevice* device) : device(device){}

const IResource* GVM::ResourcesManager::CreateBuffer(const BufferResourceDesc& desc)
{
    uint32_t resourceIndex = freeIndex++; 
    IResource* resourcePointer = (IResource*)resourceIndex;
    BufferResourceDesc savedDesc = desc;

    assert(desc.Size > 0);
    
    if (savedDesc.initialData)
    {
        uint32_t resourceSize = savedDesc.Size;
        savedData.resize(savedData.size() + savedDesc.Size, resourceSize);
        savedDesc.initialData = savedData.data() + dataSize;
        memcpy(savedData.data() + dataSize, savedDesc.initialData, resourceSize);
        dataSize += resourceSize;
    }
    
    Resources.insert(std::make_pair(resourceIndex, GpuResource(resourcePointer, desc)));
    return resourcePointer;
}
