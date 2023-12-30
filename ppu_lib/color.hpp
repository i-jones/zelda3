#pragma once

#include <type_traits>

#include "vec.hpp"

template <typename ColorElementType>
struct TColor : public Vec<ColorElementType, 3>
{
    TColor() = default;
    TColor(ColorElementType r, ColorElementType g, ColorElementType b) : Vec<ColorElementType, 3>({r, g, b}) {}

    ColorElementType &r() { return this->elem[0]; }
    const ColorElementType &r() const { return this->elem[0]; }

    ColorElementType &g() { return this->elem[1]; }
    const ColorElementType &g() const { return this->elem[1]; }

    ColorElementType &b() { return this->elem[2]; }
    const ColorElementType &b() const { return this->elem[2]; }
};

using Color = TColor<uint8_t>;

#pragma pack(push, 1)
struct Color5Bit
{
    unsigned int red : 5;
    unsigned int green : 5;
    unsigned int blue : 5;

    static Color5Bit fromUint16(std::uint16_t value)
    {
        union
        {
            Color5Bit color;
            std::uint16_t v;
        } x;
        x.v = value;
        return x.color;
    }

    Color to8Bit() const
    {
        return Color(_5to8Bit(red), _5to8Bit(green), _5to8Bit(blue));
    }

    std::uint16_t asUint16() const
    {
        static_assert(sizeof(Color5Bit) == 2);
        static_assert(std::is_pod_v<Color5Bit>);
        return *reinterpret_cast<const std::uint16_t *>(this);
    }

private:
    static constexpr uint8_t _5to8Bit(uint8_t x)
    {
        // x * 255/31 approximation
        return (x << 3) | (x >> 2);
    }
};
#pragma pack(pop)
static_assert(sizeof(Color5Bit) == 2);

struct OutputPixelFormat : public Vec<uint8_t, 4>
{
    OutputPixelFormat() = default;
    OutputPixelFormat(const Vec<uint8_t, 4> &v) : Vec<uint8_t, 4>(v) {}
    OutputPixelFormat(Color c) : Vec({c.b(), c.g(), c.r(), 0}) {}
    OutputPixelFormat(const Vec<int, 4> &v) : Vec<uint8_t, 4>(v.cast<uint8_t>()) {}

    uint8_t &b() { return this->elem[0]; }
    uint8_t b() const { return this->elem[0]; }

    uint8_t &g() { return this->elem[1]; }
    uint8_t g() const { return this->elem[1]; }

    uint8_t &r() { return this->elem[2]; }
    uint8_t r() const { return this->elem[2]; }

    uint8_t &a() { return this->elem[3]; }
    uint8_t a() const { return this->elem[3]; }

    Color5Bit to5Bit() const
    {
        Color5Bit fiveBit;
        fiveBit.red = r() >> 3;
        fiveBit.green = g() >> 3;
        fiveBit.blue = b() >> 3;
        return fiveBit;
    }
};
static_assert(sizeof(OutputPixelFormat) == sizeof(uint32_t));
static_assert(std::is_pod_v<OutputPixelFormat>);
