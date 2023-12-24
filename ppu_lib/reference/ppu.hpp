#pragma once

#include <array>
#include <optional>

#include "../PPUBase.hpp"
#include "register.hpp"
#include "../vec.hpp"
#include "../color.hpp"
#include "../RenderBuffer.hpp"
#include "Objects.hpp"
#include "VRAM.hpp"
#include "CGRam.hpp"
#include "Window.hpp"
#include "Background.hpp"

using Pixel = Vec2<int>;

constexpr int ScreenWidth = 256;

class ReferencePPU : public PPUBase
{
public:
    ReferencePPU();
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
    using Windows = std::array<LayerWindow, kLayerIndexCount>;
    using MaybeColorPiority = std::optional<ColorWithPriority>;
    using MainScreenOutput = std::pair<std::optional<ColorWithPriority>, LayerMaskFlags>;
    using BackgroundRenders = std::array<std::optional<Background>, 4>;

    void initBus();

    OutputPixelFormat computePixel(Pixel pixel);
    void computeBackground(Pixel pixel);
    OutputPixelFormat applyColorMath(
        Pixel pixel,
        const LayerWindow &window,
        OutputPixelFormat mainScreen,
        LayerMaskFlags mainFlags,
        std::optional<OutputPixelFormat> subscreen);
    Windows computeWindowStates();
    LayerWindow getWindow(LayerIndex layer);

    BackgroundRenders getBgRenderers();
    std::optional<int> getBGBitDepth(LayerIndex layer);

    template <typename Callable>
    void renderLayer(
        Callable c,
        LayerMaskFlags mask,
        LayerIndex layerIndex,
        const LayerWindow &window,
        Pixel pixel,
        MainScreenOutput &mainOutput,
        MaybeColorPiority &subOuput);

    PpuVRam vramMem;

    IniDisp iniDisp;
    ObjSel objSel;
    OamAddL oamAddL;
    OamAddH oamAddH;
    BgModeReg bgModeReg;
    Mosaic mosaic;
    BgScreenSizes bgScreens;
    BGNameAddresses bgNameAddresses;
    BG1Offsets bg1Offsets;
    BGsOffsets bgOffsets;
    M7Sel mode7Select;

    uint8_t cgRamAddress;

    WindowSettings windowSettings;
    WindowBGLogic windowBGLogic;
    WindowObjectLogic windowObjectLogic;

    LayerFlags enableMainScreen;
    LayerFlags enableSubScreen;
    LayerFlags enableMainScreenWindow;
    LayerFlags enableSubScreenWindow;

    struct WindowPositions
    {
        uint8_t left;
        uint8_t right;
    };
    WindowPositions windowPositions[2];

    ColorWindowSelect colorWindowSelect;
    ColorAddSub colorAddSub;

    Color5Bit fixedColor;

    ScreenInitSettings screenInitSettings;

    using WriteFunc = std::function<void(uint8_t)>;
    using WriteBus = std::array<WriteFunc, 0x2133 - 0x2100 + 1>;
    WriteBus writeBus;

    OAM oam;
    VRAM vramView{vramMem};
    CGRam cgRam;

    std::optional<RenderBuffer> renderBuffer;
};