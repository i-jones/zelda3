#include "ppu_interface.h"
#include "SNESPPU.hpp"
#include "DebugPPU.hpp"
#include "PPUFixedFrameRecorder.hpp"

PpuImpl *PpuImpl_alloc()
{
    PPUBase *ppu = new PPUFixedFrameRecorder(std::make_unique<DebugPPU<SNESPPU>>(), 1);

    // PPUBase *ppu = new DebugPPU<SNESPPU>();
    PpuImpl *impl = ppu;
    return impl;
}