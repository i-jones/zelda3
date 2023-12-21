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

struct OutputPixelFormat : public Vec<uint8_t, 4>
{
    OutputPixelFormat() = default;
    OutputPixelFormat(const Vec<uint8_t, 4> &v) : Vec<uint8_t, 4>(v) {}
    OutputPixelFormat(Color c) : Vec({c.b(), c.g(), c.r(), 0}) {}

    uint8_t &b() { return this->elem[0]; }
    uint8_t b() const { return this->elem[0]; }

    uint8_t &g() { return this->elem[1]; }
    uint8_t g() const { return this->elem[1]; }

    uint8_t &r() { return this->elem[2]; }
    uint8_t r() const { return this->elem[2]; }

    uint8_t &a() { return this->elem[3]; }
    uint8_t a() const { return this->elem[3]; }
};
static_assert(sizeof(OutputPixelFormat) == sizeof(uint32_t));
static_assert(std::is_pod_v<OutputPixelFormat>);