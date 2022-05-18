#pragma once
#include <cstdint>


namespace GVM {

typedef uint32_t uint;
typedef uint64_t ulong;
typedef uint8_t byte;
typedef void* pointer;

constexpr auto BiteMask(uint8_t shift, uint8_t length) noexcept
{
    uint64_t result = 0;
    for (uint8_t i = 0; i < length; i++)
    {
        result+=1;
        result*=2;
    }
    
    return result<<(64 - shift - length);
}

auto GetBiteValue(const uint64_t& value, uint8_t& shift, uint8_t length) noexcept
{
    uint8_t result = ((value&BiteMask(shift, length))>>(64 - shift - length));
    shift += length;
    return result;
}

}
