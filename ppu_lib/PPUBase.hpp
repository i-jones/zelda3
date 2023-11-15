#pragma once

#include "ppu_interface.h"

class PPUBase : public PpuImpl
{
public:
    PPUBase();
    virtual ~PPUBase();

    virtual void reset() = 0;
    virtual void runLine(int line) = 0;
    virtual uint8_t read(uint8_t adr) = 0;
    virtual void write(uint8_t adr, uint8_t val) = 0;
    virtual void writeCGRam(const void *data, size_t size) = 0;
    virtual void writeOam(const void *data, size_t size) = 0;
    virtual void saveLoad(PpuSaveLoadFunc *func, void *context) = 0;
    virtual void beginDrawing(uint8_t *buffer, size_t pitch, uint32_t render_flags) = 0;
    virtual int getCurrentRenderScale(uint32_t render_flags) = 0;
    virtual void setMode7PerspectiveCorrection(int low, int high) = 0;
    virtual void setExtraSideSpace(int left, int right, int bottom) = 0;
    virtual uint8_t getMode() = 0;
    virtual uint8_t getExtraLeftRight() = 0;
    virtual void setExtraLeftRight(uint8_t extraLeftRight) = 0;
    virtual Ppu *getPpu() = 0;
};