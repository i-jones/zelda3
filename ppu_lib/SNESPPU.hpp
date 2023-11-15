#pragma once

#include <memory>

#include "PPUBase.hpp"

// TODO put extern C in the snes lib instead
extern "C"
{
#include "snes/ppu.h"
}

class SNESPPU : public PPUBase
{
public:
    SNESPPU();
    ~SNESPPU();

    void reset() override;
    void runLine(int line) override;
    uint8_t read(uint8_t adr) override;
    void write(uint8_t adr, uint8_t val) override;
    void writeCGRam(const void *data, size_t size) override;
    void writeOam(const void *data, size_t size) override;
    void saveLoad(PpuSaveLoadFunc *func, void *context) override;
    void beginDrawing(uint8_t *buffer, size_t pitch, uint32_t render_flags) override;
    int getCurrentRenderScale(uint32_t render_flags) override;
    void setMode7PerspectiveCorrection(int low, int high) override;
    void setExtraSideSpace(int left, int right, int bottom) override;
    uint8_t getMode() override;
    uint8_t getExtraLeftRight() override;
    void setExtraLeftRight(uint8_t extraLeftRight) override;
    Ppu *getPpu() override;

private:
    std::unique_ptr<Ppu, void (*)(Ppu *)> _ppu;
};