#pragma once

#include <span>

#include "../color.hpp"

#pragma pack(push, 1)
struct Color5Bit
{
    unsigned int red : 5;
    unsigned int green : 5;
    unsigned int blue : 5;
};
#pragma pack(pop)
static_assert(sizeof(Color5Bit) == 2);

class CGRam
{
public:
    using Storage = std::array<Color5Bit, 256>;
    static constexpr size_t SizeBytes = 512;

    template <size_t N>
    class Palette
    {
    public:
        Palette(const CGRam &data, size_t start) : cgRam(data), startIndex(start) {}
        Color operator[](std::size_t idx) const
        {
            assert(idx < N);
            auto c = cgRam[startIndex + idx];
            return Color(_5to8Bit(c.red), _5to8Bit(c.green), _5to8Bit(c.blue));
        }

    private:
        static constexpr uint8_t _5to8Bit(uint8_t x)
        {
            // x * 255/31 approximation
            return (x << 3) | (x >> 2);
        }
        const CGRam &cgRam;
        size_t startIndex;
    };

    Color5Bit operator[](size_t idx) const
    {
        return _data[idx];
    }

    Palette<16> getObjPallete(size_t idx) const
    {
        return Palette<16>(*this, 0x80 + idx * 16);
    }

    void write(std::span<const uint8_t, SizeBytes> data)
    {
        memcpy(_data.begin(), &data[0], data.size());
    }

private:
    Storage _data;
};

static_assert(sizeof(CGRam::Storage) == CGRam::SizeBytes);
