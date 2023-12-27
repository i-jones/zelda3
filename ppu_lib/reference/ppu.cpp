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

    auto windows = computeWindowStates();
    auto priorities = Priority::getPriorities(bgModeReg);
    const int y = line - 1;
    ObjectRender::getObjectsForScanline(oam, line - 1, objSel, scanlineObjects);
    auto bgRenders = getBgRenderers();
    auto matrix = getMatrix();
    for (int x = 0; x < ScreenWidth; x++)
    {
        Pixel p(x, y);
        OutputPixelFormat c = computePixel(
            windows,
            priorities,
            bgRenders,
            matrix,
            p);
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

    // Mode 7 matrix;
    writeBus[RegisterIndex(0x211B)] = [this](uint8_t data)
    {
        this->mode7Matrix.a.write(data);
    };
    writeBus[RegisterIndex(0x211C)] = [this](uint8_t data)
    {
        this->mode7Matrix.b.write(data);
    };
    writeBus[RegisterIndex(0x211d)] = [this](uint8_t data)
    {
        this->mode7Matrix.c.write(data);
    };
    writeBus[RegisterIndex(0x211e)] = [this](uint8_t data)
    {
        this->mode7Matrix.d.write(data);
    };
    writeBus[RegisterIndex(0x211f)] = [this](uint8_t data)
    {
        this->mode7Matrix.x0.write(data);
    };
    writeBus[RegisterIndex(0x2120)] = [this](uint8_t data)
    {
        this->mode7Matrix.y0.write(data);
    };

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

OutputPixelFormat ReferencePPU::computePixel(
    const Windows &windows,
    const Priority::Priorities &priorities,
    const BackgroundRenders &bgRenders,
    const Mode7FixedMatrix &m,
    Pixel pixel)
{
    Pixel objPixel = pixel;
    pixel.y() = pixel.y() + 1;

    std::array<MainScreenOutput, 6> mainScreen;
    std::array<MaybeColorPiority, 6> subScreen;
    mainScreen[5] = MainScreenOutput{ColorWithPriority{cgRam.getBGColor(), Priority::BasePriority}, kLayerMaskBack};

    // render objects
    if (true)
    {
        auto renderObjects = [&]()
        {
            return ObjectRender::renderObjectList(scanlineObjects, objPixel, objSel, vramView, cgRam, priorities.obj);
        };

        renderLayer(renderObjects,
                    kLayerMaskObj,
                    kLayerIndexObj,
                    windows[kLayerIndexObj],
                    pixel,
                    mainScreen[0],
                    subScreen[0]);
    }
    if (bgModeReg.bgMode != BgMode::Mode7)
    {
        for (int i = 0; i < 4; i++)
        {
            auto &bgRenderer = bgRenders[i];
            if (!bgRenderer)
            {
                continue;
            }
            auto renderBg = [&]()
            {
                return bgRenderer->renderPixel(pixel);
            };
            LayerIndex layerIndex = static_cast<LayerIndex>(i);
            LayerMaskFlags layermask = static_cast<LayerMaskFlags>(1 << i);
            renderLayer(renderBg,
                        layermask,
                        layerIndex,
                        windows[i],
                        pixel,
                        mainScreen[i + 1],
                        subScreen[i + 1]

            );
        }
    }
    else
    {
        for (int i = 0; i < 1; i++)
        {
            auto &bgRenderer = bgRenders[i];
            if (!bgRenderer)
            {
                continue;
            }
            auto renderBg = [&]()
            {
                return bgRenderer->renderMode7(m, pixel);
            };
            LayerIndex layerIndex = static_cast<LayerIndex>(i);
            LayerMaskFlags layermask = static_cast<LayerMaskFlags>(1 << i);
            renderLayer(renderBg,
                        layermask,
                        layerIndex,
                        windows[i],
                        pixel,
                        mainScreen[i + 1],
                        subScreen[i + 1]

            );
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
        if (!window.isActive(pixel.x()))
        {
            return OutputPixelFormat({0, 0, 0});
        }
        break;
    case ColorWindowFunction::OutsideWindow:
        if (window.isActive(pixel.x()))
        {
            return OutputPixelFormat({0, 0, 0});
        }
        break;
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
        break;
    }
    case ColorMode::Subtraction:
    {
        r = (r - otherColor.red) / divisor;
        g = (g - otherColor.green) / divisor;
        b = (b - otherColor.blue) / divisor;
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

template <typename Callable>
void ReferencePPU::renderLayer(
    Callable renderFunc,
    LayerMaskFlags mask,
    LayerIndex layerIndex,
    const LayerWindow &window,
    Pixel pixel,
    MainScreenOutput &mainOutput,
    MaybeColorPiority &subOuput)
{
    // check enable and winow flags.
    const bool windowActive = window.isActive(pixel.x());
    const bool enabledMain = (enableMainScreen[layerIndex] == LayerFlag::Enable) && (enableMainScreenWindow[layerIndex] == LayerFlag::Disable || !window.isActive(pixel.x()));
    const bool enabledSub = (colorWindowSelect.ccAddEnable == ColorWindowSource::Subscreen) && (enableSubScreen[layerIndex] == LayerFlag::Enable) && (enableSubScreenWindow[layerIndex] == LayerFlag::Disable || !window.isActive(pixel.x()));
    if (!enabledMain && !enabledSub)
    {
        // skip
        // no need to calculate pixel color
        return;
    }

    MaybeColorPiority c = renderFunc();
    if (enabledMain)
    {
        mainOutput = {c, mask};
    }
    if (enabledSub)
    {
        subOuput = c;
    }
}

struct BGLayerConfig
{
    int bitDepth;
    int numPalettes;
    bool hvScroll;
    bool hvFlip;
    bool mosaic;
    bool rotateEnlargeReduce;
    bool windowMask;
    bool screenAdditionSubstraction;
    bool colorWindow;
    bool cgDirectSelect;
    bool horizontalPsuedo512;
    bool offsetChange;
    bool horiz512Mode;
};

std::optional<int> ReferencePPU::getBGBitDepth(LayerIndex layer)
{
    switch (bgModeReg.bgMode)
    {
    case BgMode::Mode0:
    {
        return 2;
    }
    case BgMode::Mode1:
    {
        if (layer == kLayerIndexBG1 || layer == kLayerIndexBG2)
        {
            return 4;
        }
        if (layer == kLayerIndexBG3)
        {
            return 2;
        }
        return std::nullopt;
    }
    case BgMode::Mode2:
    {
        if (layer == kLayerIndexBG1 || layer == kLayerIndexBG2)
        {
            return 4;
        }
        return std::nullopt;
    }
    case BgMode::Mode3:
    {
        if (layer == kLayerIndexBG1)
        {
            return 8;
        }
        if (layer == kLayerIndexBG2)
        {
            return 4;
        }
        return std::nullopt;
    }
    case BgMode::Mode4:
    {
        if (layer == kLayerIndexBG1)
        {
            return 8;
        }
        if (layer == kLayerIndexBG2)
        {
            return 2;
        }
        return std::nullopt;
    }
    case BgMode::Mode5:
    {
        if (layer == kLayerIndexBG1)
        {
            return 4;
        }
        if (layer == kLayerIndexBG2)
        {
            return 2;
        }
        return std::nullopt;
    }
    case BgMode::Mode6:
    {
        if (layer == kLayerIndexBG1)
        {
            return 4;
        }
        return std::nullopt;
    }
    case BgMode::Mode7:
    {
        if (layer == kLayerIndexBG1)
        {
            return 8;
        }
        return std::nullopt;
    }
    }
}

ReferencePPU::BackgroundRenders ReferencePPU::getBgRenderers()
{
    BackgroundRenders bgs;
    auto &bg1 = bgs[0];
    auto &bg2 = bgs[1];
    auto &bg3 = bgs[2];
    auto &bg4 = bgs[3];

    auto priorities = Priority::getPriorities(bgModeReg);

    if (bgModeReg.bgMode == BgMode::Mode7)
    {
        Vec2<int> offset(bg1Offsets.hOffset.mode7Offset(), bg1Offsets.vOffset.mode7Offset());
        int tileAddr = bgNameAddresses.bg12.baseAddr0 << 12;

        bg1.emplace(
            BgScreen{bgScreens.bg[0], vramView},
            offset,
            tileAddr,
            8,
            priorities.bg[0],
            cgRam.getBGPalette(0, bgModeReg.bgMode),
            vramView);
    }
    else
    {
        auto bitDepth = getBGBitDepth(kLayerIndexBG1);
        if (bitDepth)
        {
            Vec2<int> offset(bg1Offsets.hOffset.offset(), bg1Offsets.vOffset.offset());
            int tileAddr = bgNameAddresses.bg12.baseAddr0 << 12;

            bg1.emplace(
                BgScreen{bgScreens.bg[0], vramView},
                offset,
                tileAddr,
                *bitDepth,
                priorities.bg[0],
                cgRam.getBGPalette(0, bgModeReg.bgMode),
                vramView);
        }
    }
    {
        auto bitDepth = getBGBitDepth(kLayerIndexBG2);
        if (bitDepth)
        {

            Vec2<int> offset(bgOffsets.bg[0].hOffset.offset(), bgOffsets.bg[0].vOffset.offset());

            int tileAddr = bgNameAddresses.bg12.baseAddr1 << 12;

            bg2.emplace(
                BgScreen{bgScreens.bg[1], vramView},
                offset,
                tileAddr,
                *bitDepth,
                priorities.bg[1],
                cgRam.getBGPalette(1, bgModeReg.bgMode),
                vramView);
        }
    }
    {
        auto bitDepth = getBGBitDepth(kLayerIndexBG3);
        if (bitDepth)
        {

            Vec2<int> offset(bgOffsets.bg[1].hOffset.offset(), bgOffsets.bg[1].vOffset.offset());

            int tileAddr = bgNameAddresses.bg34.baseAddr0 << 12;

            bg3.emplace(
                BgScreen{bgScreens.bg[2], vramView},
                offset,
                tileAddr,
                *bitDepth,
                priorities.bg[2],
                cgRam.getBGPalette(2, bgModeReg.bgMode),
                vramView);
        }
    }
    {
        auto bitDepth = getBGBitDepth(kLayerIndexBG4);
        if (bitDepth)
        {

            Vec2<int> offset(bgOffsets.bg[2].hOffset.offset(), bgOffsets.bg[2].vOffset.offset());

            int tileAddr = bgNameAddresses.bg34.baseAddr1 << 12;

            bg4.emplace(
                BgScreen{bgScreens.bg[3], vramView},
                offset,
                tileAddr,
                *bitDepth,
                priorities.bg[3],
                cgRam.getBGPalette(3, bgModeReg.bgMode),
                vramView);
        }
    }
    return bgs;
}

Mode7FixedMatrix ReferencePPU::getMatrix() const
{
    return Mode7FixedMatrix{
        mode7Matrix.a.value(),
        mode7Matrix.b.value(),
        mode7Matrix.c.value(),
        mode7Matrix.d.value(),
        Int13::fromRaw(mode7Matrix.x0.raw()),
        Int13::fromRaw(mode7Matrix.y0.raw()),
    };
}