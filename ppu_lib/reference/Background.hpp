#pragma once

#include <optional>
#include <span>

#include "register.hpp"
#include "../vec.hpp"
#include "../color.hpp"
#include "RenderOutput.hpp"
#include "VRAM.hpp"
#include "CGRam.hpp"
#include "Priority.hpp"

#pragma pack(push, 1)
struct BgScData
{
    unsigned int name : 10;
    unsigned int color : 3;
    bool bgPri : 1;
    bool hFlip : 1;
    bool vFlip : 1;
};
#pragma pack(pop)
static_assert(sizeof(BgScData) == 2);

class BgBaseScreen
{
public:
    static constexpr int Width = 32;
    static constexpr int Height = 32;
    static constexpr int ScreenSize = Width * Height;
    BgBaseScreen(const BgScData *base) : _data(base, Width * Height){};
    BgScData sample(Vec2<int> loc) const;

private:
    std::span<const BgScData, Width * Height> _data;
};

class BgScreen
{
public:
    BgScreen(BGSC settings, const VRAM &vram);
    BgScData sample(Vec2<int> loc) const;

private:
    const VRAM &_vram;
    BGSC _settings;
};

class Background
{
public:
    Background(
        BgScreen screen,
        Vec2<int> offset,
        int tileBaseAddress,
        int bitDepth,
        Priority::BGPriorities priorities,
        CGRam::DynamicPalette palette,
        const VRAM &vram);

    std::optional<ColorWithPriority> renderPixel(Vec2<int> pixel) const;

private:
    BgScreen _bgScreen;
    Vec2<int> _offset;
    int _tileBaseAddress;
    int _bitDepth;
    CGRam::DynamicPalette _palette;
    const VRAM &_vram;
    Priority::BGPriorities _priorities;
};
