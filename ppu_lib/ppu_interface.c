#include "ppu_interface.h"

#include "snes/ppu.h"

typedef struct PpuImplSnes {
    PpuImpl interface;
    Ppu* ppu;
} PpuImplSnes;

static PpuImplSnes* fromBase(PpuImpl* base) {
    return (PpuImplSnes*)base;
}

static void snes_ppu_free( PpuImpl* base) {
    PpuImplSnes* impl = fromBase(base);
    ppu_free(impl->ppu);
    free(impl);
};

static void snes_ppu_reset(PpuImpl* base) {
    PpuImplSnes* impl = fromBase(base);
    ppu_reset(impl->ppu);
}
    
static void snes_ppu_runLine(PpuImpl* base, int line) {
    PpuImplSnes* impl = fromBase(base);
    ppu_runLine(impl->ppu, line);
}

static uint8_t snes_ppu_read(PpuImpl * base, uint8_t adr) {
    PpuImplSnes* impl = fromBase(base);
    return ppu_read(impl->ppu, adr);
}

static void snes_ppu_write(PpuImpl * base, uint8_t adr, uint8_t val) {
    PpuImplSnes* impl = fromBase(base);
    ppu_write(impl->ppu, adr, val);
}

static void snes_ppu_write_cgram(PpuImpl* base, const void* src, size_t length) {
    PpuImplSnes* impl = fromBase(base);
    memcpy(impl->ppu->cgram, src, length);
}

static void snes_ppu_write_oam(PpuImpl* base, const void* src, size_t length) {
    PpuImplSnes* impl = fromBase(base);
    memcpy(impl->ppu->oam, src, length);
}

static void snes_ppu_saveLoad(PpuImpl* base, PpuSaveLoadFunc * func, void *ctx) {
    PpuImplSnes* impl = fromBase(base);
    ppu_saveload(impl->ppu, func, ctx);
}
    
static void snes_ppu_BeginDrawing(PpuImpl* base, uint8_t *buffer, size_t pitch, uint32_t render_flags) {
    PpuImplSnes* impl = fromBase(base);
    PpuBeginDrawing(impl->ppu, buffer, pitch, render_flags);
}

static int snes_ppu_GetCurrentRenderScale(PpuImpl *base, uint32_t render_flags) {
    PpuImplSnes* impl = fromBase(base);
    return PpuGetCurrentRenderScale(impl->ppu, render_flags);
}

static void snes_ppu_SetMode7PerspectiveCorrection(PpuImpl *base, int low, int high) {
    PpuImplSnes* impl = fromBase(base);
    PpuSetMode7PerspectiveCorrection(impl->ppu, low, high);
}

static void snes_ppu_SetExtraSideSpace(PpuImpl* base, int left,  int right, int bottom) {
    PpuImplSnes* impl = fromBase(base);
    PpuSetExtraSideSpace(impl->ppu, left, right, bottom);
}

static uint8_t snes_ppu_GetMode(PpuImpl* base) {
    PpuImplSnes* impl = fromBase(base);
    return impl->ppu->mode;
}

static uint8_t snes_ppu_GetExtraLeftRight(PpuImpl* base) {
    PpuImplSnes* impl = fromBase(base);
    return impl->ppu->extraLeftRight;
}
    
static void snes_ppu_SetExtraLeftRight(PpuImpl* base, uint8_t extraLeftRight) {
    PpuImplSnes* impl = fromBase(base);
    impl->ppu->extraLeftRight = extraLeftRight;
}

static Ppu* snes_ppu_getPpu(PpuImpl* base) {
     PpuImplSnes* impl = fromBase(base);
     return impl->ppu;
}

PpuImpl *PpuImpl_alloc() {
    PpuImplSnes* wrapper = calloc(sizeof(PpuImplSnes), 1);
    wrapper->ppu = ppu_init();
    // Methods
    PpuMethods* methods = &wrapper->interface.methods;
    methods->free = snes_ppu_free;
    methods->reset = snes_ppu_reset;
    methods->runLine = snes_ppu_runLine;
    methods->read = snes_ppu_read;
    methods->write = snes_ppu_write;
    methods->writeCGRam = snes_ppu_write_cgram;
    methods->writeOam = snes_ppu_write_oam;
    methods->saveLoad = snes_ppu_saveLoad;
    methods->beginDrawing = snes_ppu_BeginDrawing;
    methods->getCurrentRenderScale = snes_ppu_GetCurrentRenderScale;
    methods->setMode7PerspectiveCorrection = snes_ppu_SetMode7PerspectiveCorrection;
    methods->setExtraSideSpace = snes_ppu_SetExtraSideSpace;
    methods->getMode = snes_ppu_GetMode;
    methods->getExtraLeftRight = snes_ppu_GetExtraLeftRight;
    methods->setExtraLeftRight = snes_ppu_SetExtraLeftRight;
    methods->getPpu = snes_ppu_getPpu;

    // Memory pointers
    wrapper->interface.vram = &wrapper->ppu->vram;

    return &wrapper->interface;
}
