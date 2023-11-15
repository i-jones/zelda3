#include "PPUBase.hpp"

namespace
{
    PPUBase *toBase(PpuImpl *impl)
    {
        return static_cast<PPUBase *>(impl);
    }

    void PPUBase_free(PpuImpl *impl)
    {
        PPUBase *base = toBase(impl);
        delete base;
    }

    void PPUBase_reset(PpuImpl *impl)
    {
        PPUBase *base = toBase(impl);
        base->reset();
    }

    void PPUBase_runLine(PpuImpl *impl, int line)
    {
        PPUBase *base = toBase(impl);
        base->runLine(line);
    }

    uint8_t PPUBase_read(PpuImpl *impl, uint8_t adr)
    {
        PPUBase *base = toBase(impl);
        return base->read(adr);
    }

    void PPUBase_write(PpuImpl *impl, uint8_t adr, uint8_t val)
    {
        PPUBase *base = toBase(impl);
        base->write(adr, val);
    }

    void PPUBase_write_cgram(PpuImpl *impl, const void *src, size_t length)
    {
        PPUBase *base = toBase(impl);
        base->writeCGRam(src, length);
    }

    void PPUBase_write_oam(PpuImpl *impl, const void *src, size_t length)
    {
        PPUBase *base = toBase(impl);
        base->writeOam(src, length);
    }

    void PPUBase_saveLoad(PpuImpl *impl, PpuSaveLoadFunc *func, void *ctx)
    {
        PPUBase *base = toBase(impl);
        base->saveLoad(func, ctx);
    }

    void PPUBase_BeginDrawing(PpuImpl *impl, uint8_t *buffer, size_t pitch, uint32_t render_flags)
    {
        PPUBase *base = toBase(impl);
        base->beginDrawing(buffer, pitch, render_flags);
    }

    int PPUBase_GetCurrentRenderScale(PpuImpl *impl, uint32_t render_flags)
    {
        PPUBase *base = toBase(impl);
        return base->getCurrentRenderScale(render_flags);
    }

    void PPUBase_SetMode7PerspectiveCorrection(PpuImpl *impl, int low, int high)
    {
        PPUBase *base = toBase(impl);
        base->setMode7PerspectiveCorrection(low, high);
    }

    void PPUBase_SetExtraSideSpace(PpuImpl *impl, int left, int right, int bottom)
    {
        PPUBase *base = toBase(impl);
        base->setExtraSideSpace(left, right, bottom);
    }

    uint8_t PPUBase_GetMode(PpuImpl *impl)
    {
        PPUBase *base = toBase(impl);
        return base->getMode();
    }

    uint8_t PPUBase_GetExtraLeftRight(PpuImpl *impl)
    {
        PPUBase *base = toBase(impl);
        return base->getExtraLeftRight();
    }

    void PPUBase_SetExtraLeftRight(PpuImpl *impl, uint8_t extraLeftRight)
    {
        PPUBase *base = toBase(impl);
        return base->setExtraLeftRight(extraLeftRight);
    }

    Ppu *PPUBase_getPpu(PpuImpl *impl)
    {
        PPUBase *base = toBase(impl);
        return base->getPpu();
    }
}

PPUBase::PPUBase()
{
    methods.free = PPUBase_free;
    methods.reset = PPUBase_reset;
    methods.runLine = PPUBase_runLine;
    methods.read = PPUBase_read;
    methods.write = PPUBase_write;
    methods.writeCGRam = PPUBase_write_cgram;
    methods.writeOam = PPUBase_write_oam;
    methods.saveLoad = PPUBase_saveLoad;
    methods.beginDrawing = PPUBase_BeginDrawing;
    methods.getCurrentRenderScale = PPUBase_GetCurrentRenderScale;
    methods.setMode7PerspectiveCorrection = PPUBase_SetMode7PerspectiveCorrection;
    methods.setExtraSideSpace = PPUBase_SetExtraSideSpace;
    methods.getMode = PPUBase_GetMode;
    methods.getExtraLeftRight = PPUBase_GetExtraLeftRight;
    methods.setExtraLeftRight = PPUBase_SetExtraLeftRight;
    methods.getPpu = PPUBase_getPpu;
}

PPUBase::~PPUBase() = default;
