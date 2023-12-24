#include "CGRam.hpp"

CGRam::DynamicPalette CGRam::getBGPalette(int bgIndex, BgMode mode) const
{
    int offset = 0;
    size_t size = 0;
    switch (mode)
    {
    case BgMode::Mode1:
    case BgMode::Mode2:
    {
        if (bgIndex == 0 || bgIndex == 1)
        {
            offset = 0;
            size = 16 * 8;
        }
        else if (bgIndex == 2)
        {
            offset = 0;
            size = 4 * 8;
        }
    }
    break;
    default:
        assert(false);
        break;
    }
    return DynamicPalette({_data.data() + offset, size});
}