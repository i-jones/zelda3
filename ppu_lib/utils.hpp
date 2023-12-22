#pragma once

#include <cstdint>
#include <span>
#include <string_view>

class Utils
{
public:
    static bool areImagesEqual(std::span<const std::uint8_t> a, std::span<const std::uint8_t> b);

    static bool writeBinary(std::string_view fileName, std::span<const std::uint8_t> data);
};