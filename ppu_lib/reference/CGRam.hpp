#pragma once

#include <span>

#include "register.hpp"

#include "../color.hpp"

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
            return c.to8Bit();
        }

    private:
        const CGRam &cgRam;
        size_t startIndex;
    };

    class DynamicPalette
    {
    public:
        DynamicPalette(std::span<const Color5Bit> data) : _data(std::move(data)) {}
        Color operator[](std::size_t idx) const
        {
            assert(idx < _data.size());
            auto c = _data[idx];
            return c.to8Bit();
        }

    private:
        std::span<const Color5Bit> _data;
    };

    Color5Bit operator[](size_t idx) const
    {
        return _data[idx];
    }

    Palette<16> getObjPallete(size_t idx) const
    {
        return Palette<16>(*this, 0x80 + idx * 16);
    }

    DynamicPalette getBGPalette(int bgIndex, BgMode mode) const;

    Color getBGColor() const
    {
        return _data[0].to8Bit();
    }

    void write(std::span<const uint8_t, SizeBytes> data)
    {
        memcpy(_data.begin(), &data[0], data.size());
    }

private:
    Storage _data;
};

static_assert(sizeof(CGRam::Storage) == CGRam::SizeBytes);
