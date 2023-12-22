#pragma once

#include <cstdint>
#include <span>
#include <string_view>
#include <vector>
#include <string>

enum class CompressionType
{
    None,
    Gzip,
    Zstd
};

class Utils
{
public:
    static bool areImagesEqual(std::span<const std::uint8_t> a, std::span<const std::uint8_t> b);

    static bool writeBinary(std::string_view fileName, std::span<const std::uint8_t> data, CompressionType compression);

    static std::vector<unsigned char> loadFile(const std::string &fileName);
};