#include "PPMWriter.hpp"

#include <span>

namespace
{
    struct PixelFormat
    {
        uint8_t b, g, r, a;
    };

    static_assert(sizeof(PixelFormat) == sizeof(uint32_t));
}

bool PPMwriter::writePPMImage(std::ostream &output, const void *data, uint32_t pitch /* bytes per line*/, std::optional<std::string_view> comment)
{
    // Assume data points to a 256x224 image in RGBA format
    if (pitch != 256 * sizeof(uint32_t))
    {
        return false;
    }

    const int width = 256;
    const int height = 224;
    const int maxColorValue = 255;
    output << "P3\n";
    if (comment)
    {
        output << "# " << *comment << "\n";
    }
    output << width << " " << height << "\n";
    output << maxColorValue << "\n";

    for (int y = 0; y < height; y++)
    {
        std::span<const PixelFormat> line(reinterpret_cast<const PixelFormat *>(data) + y * 256, width);
        for (const auto &pixel : line)
        {
            output << static_cast<int>(pixel.r) << " " << static_cast<int>(pixel.g) << " " << static_cast<int>(pixel.b) << "\n";
        }
    }

    return true;
}