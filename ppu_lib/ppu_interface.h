#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef void PpuSaveLoadFunc(void *ctx, void *data, size_t data_size);

    typedef uint16_t PpuVRam[0x8000];

    struct PpuImpl;
    typedef struct PpuMethods
    {
        void (*free)(struct PpuImpl *);
        void (*reset)(struct PpuImpl *);
        void (*runLine)(struct PpuImpl *, int);
        uint8_t (*read)(struct PpuImpl *, uint8_t);
        void (*write)(struct PpuImpl *, uint8_t, uint8_t);
        void (*writeCGRam)(struct PpuImpl *, const void *, size_t);
        void (*writeOam)(struct PpuImpl *, const void *, size_t);
        void (*saveLoad)(struct PpuImpl *, PpuSaveLoadFunc *, void *);
        void (*beginDrawing)(struct PpuImpl *, uint8_t *, size_t, uint32_t);
        int (*getCurrentRenderScale)(struct PpuImpl *, uint32_t);
        void (*setMode7PerspectiveCorrection)(struct PpuImpl *, int, int);
        void (*setExtraSideSpace)(struct PpuImpl *, int, int, int);
        uint8_t (*getMode)(struct PpuImpl *);
        uint8_t (*getExtraLeftRight)(struct PpuImpl *);
        void (*setExtraLeftRight)(struct PpuImpl *, uint8_t);
        struct Ppu *(*getPpu)(struct PpuImpl *);
    } PpuMethods;

    typedef struct PpuImpl
    {
        PpuMethods methods;
        PpuVRam *vram;
    } PpuImpl;

    PpuImpl *PpuImpl_alloc();

#ifdef __cplusplus
}
#endif