#pragma once

#include <ostream>
#include <optional>

class PPMwriter
{
public:
    static bool writePPMImage(std::ostream &output, const void *data, uint32_t pitch /* bytes per line*/, std::optional<std::string_view> comment);
};