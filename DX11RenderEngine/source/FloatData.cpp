
#define FULL_WINTARD
#include "winHandler.h"

#include "FloatData.h"
#include <algorithm>
#include <sstream>
#include "CoreRenderSystem/GraphicsExceptions/GraphicsException.h"




FloatData::FloatData(unsigned int width, unsigned int height) noexcept
    :
    pBuffer(std::make_unique<Color[]>(width * height)),
    width(width),
    height(height) {}

FloatData& FloatData::operator=(FloatData&& rhs) noexcept
{
    width = rhs.width;
    height = rhs.height;
    pBuffer = std::move(rhs.pBuffer);
    rhs.pBuffer = nullptr;
    return *this;
}

FloatData::FloatData(FloatData&& source) noexcept
    :
    pBuffer(std::move(source.pBuffer)),
    width(source.width),
    height(source.height) {}

FloatData::~FloatData() {}

void FloatData::Clear(Color fillValue) noexcept
{
    memset(pBuffer.get(), fillValue, width * height * sizeof(Color));
}

void FloatData::PutPixel(unsigned int x, unsigned int y, Color c) noexcept
{
    assert(x >= 0);
    assert(y >= 0);
    assert(x < width);
    assert(y < height);
    pBuffer[y * width + x] = c;
}

FloatData::Color FloatData::GetPixel(unsigned int x, unsigned int y) const noexcept
{
    assert(x >= 0);
    assert(y >= 0);
    assert(x < width);
    assert(y < height);
    return pBuffer[y * width + x];
}

unsigned int FloatData::GetWidth() const noexcept
{
    return width;
}

unsigned int FloatData::GetHeight() const noexcept
{
    return height;
}

FloatData::Color* FloatData::GetBufferPtr() noexcept
{
    return pBuffer.get();
}

const FloatData::Color* FloatData::GetBufferPtr() const noexcept
{
    return pBuffer.get();
}

const FloatData::Color* FloatData::GetBufferPtrConst() const noexcept
{
    return pBuffer.get();
}
FloatData::Color& FloatData::operator()(uint32_t x, uint32_t y) noexcept
{
    assert(x < width);
    assert(y < height);
    return pBuffer[y * width + x];
}

void FloatData::Copy(const FloatData& src) noexcept
{
    assert(width == src.width);
    assert(height == src.height);
    memcpy(pBuffer.get(), src.pBuffer.get(), width * height * sizeof(Color));
}

FloatData::FloatData(unsigned int width, unsigned int height, std::unique_ptr<Color[]> pBufferParam) noexcept
    :
    width(width),
    height(height),
    pBuffer(std::move(pBufferParam)) {}
