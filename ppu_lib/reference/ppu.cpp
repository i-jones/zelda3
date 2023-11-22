#include "ppu.hpp"

ReferencePPU::ReferencePPU()
{
    vram = &vramMem;
}
void ReferencePPU::reset() {}

void ReferencePPU::runLine(int line) {}
uint8_t ReferencePPU::read(uint8_t adr)
{
    return 0;
}
void ReferencePPU::write(uint8_t adr, uint8_t val) {}
void ReferencePPU::writeCGRam(const void *data, size_t size) {}
void ReferencePPU::writeOam(const void *data, size_t size) {}
void ReferencePPU::saveLoad(PpuSaveLoadFunc *func, void *context) {}
void ReferencePPU::beginDrawing(uint8_t *buffer, size_t pitch, uint32_t render_flags) {}
int ReferencePPU::getCurrentRenderScale(uint32_t render_flags)
{
    return 1;
}
void ReferencePPU::setMode7PerspectiveCorrection(int low, int high) {}
void ReferencePPU::setExtraSideSpace(int left, int right, int bottom) {}
uint8_t ReferencePPU::getMode()
{
    return 1;
}
uint8_t ReferencePPU::getExtraLeftRight() { return 0; }
void ReferencePPU::setExtraLeftRight(uint8_t extraLeftRight) {}
Ppu *ReferencePPU::getPpu()
{
    return nullptr;
}