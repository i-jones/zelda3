#pragma once

#include <cstdint>
#include <span>

class Utils
{
public:
    static bool areImagesEqual(std::span<const std::uint8_t> a, std::span<const std::uint8_t> b);
};