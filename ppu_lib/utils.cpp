#include "utils.hpp"

#include <algorithm>

bool Utils::areImagesEqual(std::span<const std::uint8_t> a, std::span<const std::uint8_t> b)
{
    return std::equal(a.begin(), a.end(), b.begin(), b.end());
}