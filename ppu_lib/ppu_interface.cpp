#include "ppu_interface.h"
#include "SNESPPU.hpp"
#include "DebugPPU.hpp"
#include "PPUFixedFrameRecorder.hpp"
#include "ComparisonPPU.hpp"
#include "reference/ppu.hpp"

PpuImpl *PpuImpl_alloc()
{
    // PPUBase *ppu = new PPUFixedFrameRecorder(std::make_unique<DebugPPU<SNESPPU>>(), 5500, true, "test");
    /*
        PPUBase *ppu = new ComparisonPPU(
            std::make_unique<SNESPPU>(),
            std::make_unique<ReferencePPU>());
            */
    // PPUBase *ppu = new DebugPPU<SNESPPU>();
    PPUBase *ppu = new SNESPPU();
    // PPUBase *ppu = new ReferencePPU();
    PpuImpl *impl = ppu;
    return impl;
}