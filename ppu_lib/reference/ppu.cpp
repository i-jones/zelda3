#include "ppu.hpp"

#include <algorithm>
#include <iostream>

#include "Background.hpp"
#include "Priority.hpp"

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
    if (line == 0)
        return;
    for (int x = 0; x < ScreenWidth; x++)
    {
        Pixel p(x, line - 1);
        OutputPixelFormat c = computePixel(p);
        c = (c * iniDisp.fade / 15).cast<uint8_t>();
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
void ReferencePPU::writeCGRam(const void *data, size_t size)
{
    assert(size == 512);

    cgRam.write(std::span<const uint8_t, 512>(reinterpret_cast<const uint8_t *>(data), size));
}
void ReferencePPU::writeOam(const void *data, size_t size)
{
    assert(size == sizeof(OAM));
    memcpy(&oam, data, size);
}
void ReferencePPU::saveLoad(PpuSaveLoadFunc *func, void *context) {}
void ReferencePPU::beginDrawing(uint8_t *buffer, size_t pitch, uint32_t render_flags)
{
    renderBuffer = RenderBuffer(256, 224, buffer, pitch);
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

            // std::cerr << "Write to " << std::hex << (i + 0x2100) << " not implemented!\n";
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
            this->fixedColor.red = coldata.colorBrillianceData;
        }
        if (coldata.green)
        {
            this->fixedColor.green = coldata.colorBrillianceData;
        }
        if (coldata.blue)
        {
            this->fixedColor.blue = coldata.colorBrillianceData;
        }
    };
    writeBus[RegisterIndex(0x2133)] = [this](uint8_t data)
    {
        this->screenInitSettings.write(data);
    };
}

using MainScreenOutput = std::pair<std::optional<ColorWithPriority>, LayerMaskFlags>;

OutputPixelFormat ReferencePPU::computePixel(Pixel pixel)
{
    auto windows = computeWindowStates();
    Pixel objPixel = pixel;
    pixel.y() = pixel.y() + 1;
    using MaybeColorPiority = std::optional<ColorWithPriority>;
    std::array<MainScreenOutput, 6> mainScreen;
    std::array<MaybeColorPiority, 6> subScreen;
    mainScreen[5] = MainScreenOutput{ColorWithPriority{cgRam.getBGColor(), Priority::BasePriority}, kLayerMaskBack};

    auto priorities = Priority::getPriorities(bgModeReg);

    // render objects
    if (enableMainScreen.obj == LayerFlag::Enable || enableSubScreen.obj == LayerFlag::Enable)
    {
        auto objResult = ObjectRender::renderObjects(oam, objPixel, objSel, vramView, cgRam, priorities.obj);
        if (enableMainScreen.obj == LayerFlag::Enable)
        {
            mainScreen[0] = {objResult, kLayerMaskObj};
        }
        if (enableSubScreen.obj == LayerFlag::Enable)
        {
            subScreen[0] = objResult;
        }
    }

    if (bgModeReg.bgMode == BgMode::Mode1)
    {
        // BG 1
        // if (false)
        {
            Vec2<int> offset(bg1Offsets.hOffset.offset(), bg1Offsets.vOffset.offset());
            int tileAddr = bgNameAddresses.bg12.baseAddr0 << 12;
            int bitDepth = 4;
            Background bg1(
                {bgScreens.bg[0], vramView},
                offset,
                tileAddr,
                bitDepth,
                priorities.bg[0],
                cgRam.getBGPalette(0, bgModeReg.bgMode),
                vramView);
            auto result = bg1.renderPixel(pixel);
            if (enableMainScreen.bg1 == LayerFlag::Enable)
            {
                mainScreen[1] = {result, kLayerMaskBg1};
            }
            if (enableSubScreen.bg1 == LayerFlag::Enable)
            {
                subScreen[1] = result;
            }
        }
        // BG 2
        if (true)
        {
            Vec2<int> offset(bgOffsets.bg[0].hOffset.offset(), bgOffsets.bg[0].vOffset.offset());
            int tileAddr = bgNameAddresses.bg12.baseAddr1 << 12;
            int bitDepth = 4;
            Background bg2(
                {bgScreens.bg[1], vramView},
                offset,
                tileAddr,
                bitDepth,
                priorities.bg[1],
                cgRam.getBGPalette(1, bgModeReg.bgMode),
                vramView);
            auto result = bg2.renderPixel(pixel);
            if (enableMainScreen.bg2 == LayerFlag::Enable)
            {
                mainScreen[2] = {result, kLayerMaskBg2};
            }
            if (enableSubScreen.bg2 == LayerFlag::Enable)
            {
                subScreen[2] = result;
            }
        }
        // BG 3
        // if (false)
        {
            Vec2<int> offset(bgOffsets.bg[1].hOffset.offset(), bgOffsets.bg[1].vOffset.offset());
            int tileAddr = bgNameAddresses.bg34.baseAddr0 << 12;
            int bitDepth = 2;
            Background bg3(
                {bgScreens.bg[2], vramView},
                offset,
                tileAddr,
                bitDepth,
                priorities.bg[2],
                cgRam.getBGPalette(2, bgModeReg.bgMode),
                vramView);
            auto result = bg3.renderPixel(pixel);
            if (enableMainScreen.bg3 == LayerFlag::Enable)
            {
                mainScreen[3] = {result, kLayerMaskBg3};
            }
            if (enableSubScreen.bg3 == LayerFlag::Enable)
            {
                subScreen[3] = result;
            }
        }
    }
    auto compare = [](const MaybeColorPiority &a, const MaybeColorPiority &b)
    {
        if (b && !a) {
            return true;
        } else if (b && b->priority > a ->priority) {
            return true;
        }
        return false; };
    auto compareMain = [compare](const MainScreenOutput &a, const MainScreenOutput &b)
    { return compare(a.first, b.first); };

    auto mainMaxIt = std::max_element(mainScreen.begin(), mainScreen.end(), compareMain);
    assert(mainMaxIt != mainScreen.end());
    auto subMaxIt = std::max_element(subScreen.begin(), subScreen.end(), compare);
    assert(subMaxIt != subScreen.end());

    std::optional<OutputPixelFormat> subscreenValue;
    if (*subMaxIt)
    {
        subscreenValue = subMaxIt->value().color;
    }
    return applyColorMath(
        pixel,
        windows[kLayerIndexBack],
        mainMaxIt->first->color,
        mainMaxIt->second,
        subscreenValue);
    return OutputPixelFormat(mainMaxIt->first->color);
}

OutputPixelFormat ReferencePPU::applyColorMath(
    Pixel pixel,
    const LayerWindow &window,
    OutputPixelFormat mainScreen,
    LayerMaskFlags mainFlags,
    std::optional<OutputPixelFormat> subscreen)
{
    int py = pixel.y();
    // disabled for layer
    if (!(colorAddSub.enableMask & mainFlags))
    {
        return mainScreen;
    }

    Color5Bit otherColor = fixedColor;
    if (colorWindowSelect.ccAddEnable == ColorWindowSource::Subscreen && subscreen)
    {
        otherColor = subscreen->to5Bit();
    }
    // TODO check window
    switch (colorWindowSelect.main)
    {
    case ColorWindowFunction::Off:
        return OutputPixelFormat({0, 0, 0});
    case ColorWindowFunction::InsideWindow:
    case ColorWindowFunction::OutsideWindow:
        break; // return mainScreen;
    case ColorWindowFunction::On:
        break;
    }
    switch (colorWindowSelect.sub)
    {
    case ColorWindowFunction::Off:
        return mainScreen;
    case ColorWindowFunction::On:
        break;
    case ColorWindowFunction::InsideWindow:
        if (!window.isActive(pixel.x()))
        {
            return mainScreen;
        }
        break;
    case ColorWindowFunction::OutsideWindow:
        if (window.isActive(pixel.x()))
        {
            return mainScreen;
        }
        break;
    }
    // OutputPixelFormat result = mainScreen;
    // Vec<int, 4> intermediateColor;
    Color5Bit intermediateColor;
    int divisor = colorAddSub.halfEnable ? 2 : 1;
    Color5Bit main = mainScreen.to5Bit();
    int r = main.red;
    int g = main.green;
    int b = main.blue;
    switch (colorAddSub.addSub)
    {
    case ColorMode::Addition:
    {
        r = (r + otherColor.red) / divisor;
        g = (g + otherColor.green) / divisor;
        b = (b + otherColor.blue) / divisor;
        // intermediateColor = (mainScreen + otherColor) / divisor;
        break;
    }
    case ColorMode::Subtraction:
    {
        r = (r - otherColor.red) / divisor;
        g = (g - otherColor.green) / divisor;
        b = (b - otherColor.blue) / divisor;
        // intermediateColor = (mainScreen - otherColor) / divisor;
        break;
    }
    }

    r = std::clamp(r, 0, 31);
    g = std::clamp(g, 0, 31);
    b = std::clamp(b, 0, 31);
    Color5Bit result;
    result.red = r;
    result.green = g;
    result.blue = b;
    return result.to8Bit();
}

void ReferencePPU::computeBackground(Pixel pixel)
{
}

ReferencePPU::Windows ReferencePPU::computeWindowStates()
{
    Windows windows;
    for (int i = 0; i < windows.size(); i++)
    {
        windows[i] = getWindow(static_cast<LayerIndex>(i));
    }
    return windows;
}

LayerWindow ReferencePPU::getWindow(LayerIndex layer)
{
    LayerWindow result;
    auto &windows = result.windows;
    windows[0].left = windowPositions[0].left;
    windows[0].right = windowPositions[0].right;
    windows[1].left = windowPositions[1].left;
    windows[1].right = windowPositions[1].right;
    switch (layer)
    {
    case kLayerIndexBG1:
    {
        windows[0].enabled = windowSettings.windowBg12.bg1Enable1 == WindowEnabled::On;
        windows[0].mode = windowSettings.windowBg12.bg1InOut1;

        windows[1].enabled = windowSettings.windowBg12.bg1Enable2 == WindowEnabled::On;
        windows[1].mode = windowSettings.windowBg12.bg1InOut2;

        result.logic = windowBGLogic.bg1;

        break;
    }
    case kLayerIndexBG2:
    {
        windows[0].enabled = windowSettings.windowBg12.bg2Enable1 == WindowEnabled::On;
        windows[0].mode = windowSettings.windowBg12.bg2InOut1;

        windows[1].enabled = windowSettings.windowBg12.bg2Enable2 == WindowEnabled::On;
        windows[1].mode = windowSettings.windowBg12.bg2InOut2;

        result.logic = windowBGLogic.bg2;

        break;
    }
    case kLayerIndexBG3:
    {
        windows[0].enabled = windowSettings.windowBg34.bg1Enable1 == WindowEnabled::On;
        windows[0].mode = windowSettings.windowBg34.bg1InOut1;

        windows[1].enabled = windowSettings.windowBg34.bg1Enable2 == WindowEnabled::On;
        windows[1].mode = windowSettings.windowBg34.bg1InOut2;

        result.logic = windowBGLogic.bg3;

        break;
    }
    case kLayerIndexBG4:
    {
        windows[0].enabled = windowSettings.windowBg34.bg2Enable1 == WindowEnabled::On;
        windows[0].mode = windowSettings.windowBg34.bg2InOut1;

        windows[1].enabled = windowSettings.windowBg34.bg2Enable2 == WindowEnabled::On;
        windows[1].mode = windowSettings.windowBg34.bg2InOut2;

        result.logic = windowBGLogic.bg4;

        break;
    }
    case kLayerIndexObj:
    {
        windows[0].enabled = windowSettings.windowObjColor.bg1Enable1 == WindowEnabled::On;
        windows[0].mode = windowSettings.windowObjColor.bg1InOut1;

        windows[1].enabled = windowSettings.windowObjColor.bg1Enable2 == WindowEnabled::On;
        windows[1].mode = windowSettings.windowObjColor.bg1InOut2;

        result.logic = windowObjectLogic.obj;
        break;
    }
    case kLayerIndexBack:
    {
        windows[0].enabled = windowSettings.windowObjColor.bg2Enable1 == WindowEnabled::On;
        windows[0].mode = windowSettings.windowObjColor.bg2InOut1;

        windows[1].enabled = windowSettings.windowObjColor.bg2Enable2 == WindowEnabled::On;
        windows[1].mode = windowSettings.windowObjColor.bg2InOut2;

        result.logic = windowObjectLogic.color;
        break;
    }
    default:
        assert(false);
        break;
    }

    return result;
}
