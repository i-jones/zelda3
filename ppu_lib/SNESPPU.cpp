#include "SNESPPU.hpp"

SNESPPU::SNESPPU() : _ppu(ppu_init(), ppu_free)
{
    vram = &_ppu->vram;
}

SNESPPU::~SNESPPU() = default;

void SNESPPU::reset()
{
    ppu_reset(_ppu.get());
}

void SNESPPU::runLine(int line)
{
    ppu_runLine(_ppu.get(), line);
}

uint8_t SNESPPU::read(uint8_t adr)
{
    return ppu_read(_ppu.get(), adr);
}

void SNESPPU::write(uint8_t adr, uint8_t val)
{
    ppu_write(_ppu.get(), adr, val);
}

void SNESPPU::writeCGRam(const void *data, size_t size)
{
    memcpy(_ppu->cgram, data, size);
}

void SNESPPU::writeOam(const void *data, size_t size)
{
    memcpy(_ppu->oam, data, size);
}

void SNESPPU::saveLoad(PpuSaveLoadFunc *func, void *context)
{
    ppu_saveload(_ppu.get(), func, context);
}

void SNESPPU::beginDrawing(uint8_t *buffer, size_t pitch, uint32_t render_flags)
{
    PpuBeginDrawing(_ppu.get(), buffer, pitch, render_flags);
}

int SNESPPU::getCurrentRenderScale(uint32_t render_flags)
{
    return PpuGetCurrentRenderScale(_ppu.get(), render_flags);
}

void SNESPPU::setMode7PerspectiveCorrection(int low, int high)
{
    PpuSetMode7PerspectiveCorrection(_ppu.get(), low, high);
}

void SNESPPU::setExtraSideSpace(int left, int right, int bottom)
{
    PpuSetExtraSideSpace(_ppu.get(), left, right, bottom);
}

uint8_t SNESPPU::getMode()
{
    return _ppu->mode;
}

uint8_t SNESPPU::getExtraLeftRight()
{
    return _ppu->extraLeftRight;
}

void SNESPPU::setExtraLeftRight(uint8_t extraLeftRight)
{
    _ppu->extraLeftRight = extraLeftRight;
}

Ppu *SNESPPU::getPpu()
{
    return _ppu.get();
}