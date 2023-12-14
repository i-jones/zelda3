#pragma once

#include <span>

#include "color.hpp"

class RenderBuffer
{
public:
    using Row = std::span<OutputPixelFormat>;
    using ConstRow = std::span<const OutputPixelFormat>;

    RenderBuffer(int width, int height, void *data, uint32_t pitch /* bytes per line*/);

    void writePixel(Vec2<int> pixel, OutputPixelFormat color);

    Row row(int r);
    ConstRow row(int r) const;

private:
    int _width;
    int _height;
    std::span<OutputPixelFormat> _data;
};