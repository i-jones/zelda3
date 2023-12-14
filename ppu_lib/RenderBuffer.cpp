#include "RenderBuffer.hpp"

RenderBuffer::RenderBuffer(int width, int height, void *data, uint32_t pitch /* bytes per line*/)
    : _width(width), _height(height), _data(reinterpret_cast<OutputPixelFormat *>(data), (pitch * height) / sizeof(OutputPixelFormat))
{
}
void RenderBuffer::writePixel(Vec2<int> pixel, OutputPixelFormat color)
{
    assert(pixel.y() >= 0 && pixel.y() < _height);
    assert(pixel.x() >= 0 && pixel.x() < _width);
    row(pixel.y())[pixel.x()] = color;
}

RenderBuffer::Row RenderBuffer::row(int r)
{
    return Row(&_data[r * _width], _width);
}

RenderBuffer::ConstRow RenderBuffer::row(int r) const
{
    return ConstRow(&_data[r * _width], _width);
}