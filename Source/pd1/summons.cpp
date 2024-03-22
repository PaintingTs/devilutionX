#include "pd1/summons.h"

#include <array>
#include <cstdint>

// TODO: temporary solution to not bother with resource files
constexpr std::array<uint8_t, 256> SkeletonTrn = []() constexpr
{
    std::array<uint8_t, 256> arr{0};
    for (unsigned i = 0; i < 256; ++i)
        arr[i] = (uint8_t)i;

    for (unsigned i = 0xA1; i <= 0xAF; ++i) // A1-AF => F0-FE
        arr[i] += 0x50 - 1;

    for (unsigned i = 0xD0; i <= 0xDF; ++i) // D0-DF => C0-CF probably not needed
        arr[i] -= 0x10;

    for (unsigned i = 0xE0; i <= 0xEF; ++i) // E0-EF => F0-FF probably not needed
        arr[i] += 0x10;
    
    arr[0xEF] = arr[0xFF] = 0;
    return arr;
}();