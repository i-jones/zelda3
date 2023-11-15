#include "ppu_interface.h"
#include "SNESPPU.hpp"

PpuImpl *PpuImpl_alloc()
{
    PPUBase *ppu = new SNESPPU();
    PpuImpl *impl = ppu;
    return impl;
}