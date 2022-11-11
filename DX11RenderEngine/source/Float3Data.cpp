
#define FULL_WINTARD
#include "winHandler.h"

#include "Float3Data.h"
#include <algorithm>
#include <sstream>
#include "CoreRenderSystem/GraphicsExceptions/GraphicsException.h"




Float3Data::Float3Data(unsigned int width, unsigned int height) noexcept
    :
    pBuffer(std::make_unique<Color[]>(width * height)),
    width(width),
    height(height) {}

Float3Data& Float3Data::operator=(Float3Data&& rhs) noexcept
{
    width = rhs.width;
    height = rhs.height;
    pBuffer = std::move(rhs.pBuffer);
    rhs.pBuffer = nullptr;
    return *this;
}

Float3Data::Float3Data(Float3Data&& source) noexcept
    :
    pBuffer(std::move(source.pBuffer)),
    width(source.width),
    height(source.height) {}

Float3Data::~Float3Data() {}

void Float3Data::Clear(Color fillValue) noexcept
{
    for (uint32_t x = 0 ; x < GetWidth(); x++)
    for (uint32_t y = 0 ; y < GetHeight(); y++)
    (*this)(x,y) = fillValue;
    //memset(pBuffer.get(), fillValue, width * height * sizeof(Color));
}

void Float3Data::PutPixel(unsigned int x, unsigned int y, Color c) noexcept
{
    assert(x < width);
    assert(y < height);
    pBuffer[y * width + x] = c;
}

Float3Data::Color Float3Data::GetPixel(unsigned int x, unsigned int y) const noexcept
{
    assert(x < width);
    assert(y < height);
    return pBuffer[y * width + x];
}

unsigned int Float3Data::GetWidth() const noexcept
{
    return width;
}

unsigned int Float3Data::GetHeight() const noexcept
{
    return height;
}

Float3Data::Color* Float3Data::GetBufferPtr() noexcept
{
    return pBuffer.get();
}

const Float3Data::Color* Float3Data::GetBufferPtr() const noexcept
{
    return pBuffer.get();
}

const Float3Data::Color* Float3Data::GetBufferPtrConst() const noexcept
{
    return pBuffer.get();
}
Float3Data::Color& Float3Data::operator()(uint32_t x, uint32_t y) noexcept
{
    assert(x < width);
    assert(y < height);
    return pBuffer[y * width + x];
}

void Float3Data::Copy(const Float3Data& src) noexcept
{
    assert(width == src.width);
    assert(height == src.height);
    memcpy(pBuffer.get(), src.pBuffer.get(), width * height * sizeof(Color));
}

Float3Data::Float3Data(unsigned int width, unsigned int height, std::unique_ptr<Color[]> pBufferParam) noexcept
    :
    width(width),
    height(height),
    pBuffer(std::move(pBufferParam)) {}
