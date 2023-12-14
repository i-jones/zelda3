#include "ppu.hpp"

#include <iostream>

ReferencePPU::ReferencePPU()
{
    vram = &vramMem;
    initBus();
}
void ReferencePPU::reset()
{
    objSel.nameBaseAddr = 2;
    objSel.nameSelect = 0;
}

void ReferencePPU::runLine(int line)
{
    for (int x = 0; x < ScreenWidth; x++)
    {
        Pixel p(x, line);
        OutputPixelFormat c = computePixel(p);
        // Write color to output buffer
        renderBuffer->writePixel(p, c);
    }
}
uint8_t ReferencePPU::read(uint8_t adr)
{
    return 0;
}
void ReferencePPU::write(uint8_t adr, uint8_t val)
{
    if (adr >= writeBus.size())
    {
        std::abort();
    }
    writeBus[adr](val);
}
void ReferencePPU::writeCGRam(const void *data, size_t size) {}
void ReferencePPU::writeOam(const void *data, size_t size)
{
    assert(size == sizeof(OAM));
    memcpy(&oam, data, size);
}
void ReferencePPU::saveLoad(PpuSaveLoadFunc *func, void *context) {}
void ReferencePPU::beginDrawing(uint8_t *buffer, size_t pitch, uint32_t render_flags)
{
    renderBuffer = RenderBuffer(256, 225, buffer, pitch);
}
int ReferencePPU::getCurrentRenderScale(uint32_t render_flags)
{
    return 1;
}
void ReferencePPU::setMode7PerspectiveCorrection(int low, int high) {}
void ReferencePPU::setExtraSideSpace(int left, int right, int bottom) {}
uint8_t ReferencePPU::getMode()
{
    return static_cast<uint8_t>(bgModeReg.bgMode);
}
uint8_t ReferencePPU::getExtraLeftRight() { return 0; }
void ReferencePPU::setExtraLeftRight(uint8_t extraLeftRight) {}
Ppu *ReferencePPU::getPpu()
{
    return nullptr;
}

void ReferencePPU::initBus()
{
    for (int i = 0; i < writeBus.size(); i++)
    {
        writeBus[i] = [i](uint8_t)
        {
            auto snesAdr = i + 0x2100;

            std::cerr << "Write to " << std::hex << (i + 0x2100) << " not implemented!\n";
        };
    }

    writeBus[RegisterIndex(0x2100)] = [this](uint8_t data)
    {
        this->iniDisp.write(data);
    };
    writeBus[RegisterIndex(0x2101)] = [this](uint8_t data)
    {
        this->objSel.write(data);
    };
    writeBus[RegisterIndex(0x2102)] = [this](uint8_t data)
    {
        // TODO this changes depending on the value in oamAddH.
        this->oamAddL.write(data);
    };
    writeBus[RegisterIndex(0x2103)] = [this](uint8_t data)
    {
        this->oamAddH.write(data);
    };
    // TODO 2104
    writeBus[RegisterIndex(0x2105)] = [this](uint8_t data)
    {
        this->bgModeReg.write(data);
    };
    writeBus[RegisterIndex(0x2106)] = [this](uint8_t data)
    {
        this->mosaic.write(data);
    };
    for (int i = 0; i < 4; i++)
    {
        writeBus[RegisterIndex(0x2107 + i)] = [this, i](uint8_t data)
        {
            this->bgScreens.bg[i].write(data);
        };
    }
    writeBus[RegisterIndex(0x210B)] = [this](uint8_t data)
    {
        this->bgNameAddresses.bg12.write(data);
    };
    writeBus[RegisterIndex(0x210C)] = [this](uint8_t data)
    {
        this->bgNameAddresses.bg34.write(data);
    };
    writeBus[RegisterIndex(0x210D)] = [this](uint8_t data)
    {
        this->bg1Offsets.hOffset.write(data);
    };
    writeBus[RegisterIndex(0x210E)] = [this](uint8_t data)
    {
        this->bg1Offsets.vOffset.write(data);
    };
    for (int i = 0; i < 3; i++)
    {
        writeBus[RegisterIndex(0x210F + i * 2)] = [this, i](uint8_t data)
        {
            this->bgOffsets.bg[i].hOffset.write(data);
        };
        writeBus[RegisterIndex(0x210F + i * 2 + 1)] = [this, i](uint8_t data)
        {
            this->bgOffsets.bg[i].vOffset.write(data);
        };
    }
    // TODO 2115
    // TODO 2116
    // TODO 2117
    // TODO 2118
    // TODO 2119
    writeBus[RegisterIndex(0x211A)] = [this](uint8_t data)
    {
        this->mode7Select.write(data);
    };
    // TODO 211B mode7 A
    // TODO 211C mode7 B
    // TODO 211D mode7 C
    // TODO 211E mode7 D
    // TODO 211F mode7 X
    // TODO 2120 mode7 Y

    writeBus[RegisterIndex(0x2121)] = [this](uint8_t data)
    {
        this->cgRamAddress = data;
    };

    // TODO 2122 CGDATA

    writeBus[RegisterIndex(0x2123)] = [this](uint8_t data)
    {
        this->windowSettings.windowBg12.write(data);
    };

    writeBus[RegisterIndex(0x2124)] = [this](uint8_t data)
    {
        this->windowSettings.windowBg34.write(data);
    };

    writeBus[RegisterIndex(0x2125)] = [this](uint8_t data)
    {
        this->windowSettings.windowObjColor.write(data);
    };

    writeBus[RegisterIndex(0x2126)] = [this](uint8_t data)
    {
        this->windowPositions[0].left = data;
    };
    writeBus[RegisterIndex(0x2127)] = [this](uint8_t data)
    {
        this->windowPositions[0].right = data;
    };
    writeBus[RegisterIndex(0x2128)] = [this](uint8_t data)
    {
        this->windowPositions[1].left = data;
    };
    writeBus[RegisterIndex(0x2129)] = [this](uint8_t data)
    {
        this->windowPositions[1].right = data;
    };
    writeBus[RegisterIndex(0x212A)] = [this](uint8_t data)
    {
        this->windowBGLogic.write(data);
    };
    writeBus[RegisterIndex(0x212B)] = [this](uint8_t data)
    {
        this->windowObjectLogic.write(data);
    };
    writeBus[RegisterIndex(0x212C)] = [this](uint8_t data)
    {
        this->enableMainScreen.write(data);
    };
    writeBus[RegisterIndex(0x212D)] = [this](uint8_t data)
    {
        this->enableSubScreen.write(data);
    };
    writeBus[RegisterIndex(0x212E)] = [this](uint8_t data)
    {
        this->enableMainScreenWindow.write(data);
    };
    writeBus[RegisterIndex(0x212F)] = [this](uint8_t data)
    {
        this->enableSubScreenWindow.write(data);
    };
    writeBus[RegisterIndex(0x2130)] = [this](uint8_t data)
    {
        this->colorWindowSelect.write(data);
    };
    writeBus[RegisterIndex(0x2131)] = [this](uint8_t data)
    {
        this->colorAddSub.write(data);
    };
    writeBus[RegisterIndex(0x2132)] = [this](uint8_t data)
    {
        ColorData coldata;
        coldata.write(data);
        if (coldata.red)
        {
            this->fixedColor.r() = coldata.colorBrillianceData;
        }
        if (coldata.green)
        {
            this->fixedColor.g() = coldata.colorBrillianceData;
        }
        if (coldata.blue)
        {
            this->fixedColor.b() = coldata.colorBrillianceData;
        }
    };
    writeBus[RegisterIndex(0x2133)] = [this](uint8_t data)
    {
        this->screenInitSettings.write(data);
    };
}

OutputPixelFormat ReferencePPU::computePixel(Pixel pixel)
{
    auto objResult = ObjectRender::renderObjects(oam, pixel, objSel, vramView);
    if (objResult)
    {
        return OutputPixelFormat(objResult->color);
    }
    return OutputPixelFormat(fixedColor);
}

void ReferencePPU::computeBackground(Pixel pixel)
{
}
