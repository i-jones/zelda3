#pragma once
#include "../vec.hpp"

#pragma pack(push, 1)
struct BitPlaneRow
{
    uint8_t low;
    uint8_t high;
};
#pragma pack(pop)
static_assert(sizeof(BitPlaneRow) == 2);

template <unsigned int N>
#pragma pack(push, 1)
struct CharData
{
    static constexpr unsigned int NumDoublePlanes = N / 2;
    BitPlaneRow bitplaneData[NumDoublePlanes][8];

    unsigned int sample(Vec2<int> loc) const
    {
        unsigned int rowShift = 7 - loc.x();
        unsigned int result = 0;
        for (int p = NumDoublePlanes - 1; p >= 0; p--)
        {
            auto row = bitplaneData[p][loc.y()];
            unsigned int bit = (row.high >> rowShift) & 1;
            result <<= 1;
            result |= bit;
            bit = (row.low >> rowShift) & 1;
            result <<= 1;
            result |= bit;
        }
        assert(result < (1 << N));
        return result;
    }
};
#pragma pack(pop)
static_assert(sizeof(CharData<2>) == 16);
static_assert(sizeof(CharData<4>) == 32);
static_assert(sizeof(CharData<8>) == 64);
