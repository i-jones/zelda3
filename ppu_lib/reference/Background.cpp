#include "Background.hpp"

#include "CharData.hpp"

BgScData BgBaseScreen::sample(Vec2<int> loc) const
{
    int tileX = loc.x() / 8;
    int tileY = loc.y() / 8;
    int index = tileY * Width + tileX;
    return _data[index];
}

BgScreen::BgScreen(BGSC settings, const VRAM &vram) : _settings(settings), _vram(vram){};

BgScData BgScreen::sample(Vec2<int> loc) const
{

    constexpr int screenWidth = BgBaseScreen::Width * 8;
    constexpr int screenHeight = BgBaseScreen::Height * 8;
    size_t screenOffset = 0;
    switch (_settings.scSize)
    {
    case BgScreenSize::ScreenSize00:
        screenOffset = 0;
        break;
    case BgScreenSize::ScreenSize01:
        screenOffset = (loc.x() / screenWidth % 2) ? 0x400 : 0;
        break;
    case BgScreenSize::ScreenSize10:
        screenOffset = (loc.y() / screenHeight % 2) ? 0x400 : 0;
        break;
    case BgScreenSize::ScreenSize11:
        screenOffset = (loc.x() / screenWidth % 2) ? 0x400 : 0;
        screenOffset += (loc.y() / screenHeight % 2) ? 0x800 : 0;
        break;
    }
    Vec2<int> localPos({loc.x() % screenWidth, loc.y() % screenHeight});

    auto screenAddr = (_settings.scBaseAddress << 10) + screenOffset;
    BgBaseScreen screen(&_vram.readAs<BgScData>(screenAddr));
    return screen.sample(localPos);
}

Background::Background(
    BgScreen screen,
    Vec2<int> offset,
    int tileBaseAddress,
    int bitDepth,
    Priority::BGPriorities priorities,
    CGRam::DynamicPalette palette,
    const VRAM &vram)
    : _bgScreen(std::move(screen)),
      _offset(offset),
      _tileBaseAddress(tileBaseAddress),
      _palette(std::move(palette)),
      _bitDepth(bitDepth),
      _vram(vram),
      _priorities(priorities) {}

std::optional<ColorWithPriority> Background::renderPixel(Vec2<int> pixel) const
{
    // Convert pixel location to bg local location
    int px = pixel.x();
    int py = pixel.y();
    Vec2<int> bgLoc = pixel + _offset;
    // sample tile info
    auto tile = _bgScreen.sample(bgLoc);

    // compute local tile coords
    Vec2<int> tileLoc(bgLoc.x() % 8, bgLoc.y() % 8);
    if (tile.hFlip)
    {
        tileLoc.x() = 7 - tileLoc.x();
    }
    if (tile.vFlip)
    {
        tileLoc.y() = 7 - tileLoc.y();
    }

    auto charddress = _tileBaseAddress + tile.name * 4 * _bitDepth;
    int colorIndex = 0;
    int paletteSize = 1 << _bitDepth;
    switch (_bitDepth)
    {
    case 2:
    {
        const auto &tile = _vram.readAs<CharData<2>>(charddress);
        colorIndex = tile.sample(tileLoc);
        break;
    }
    case 4:
    {
        const auto &tile = _vram.readAs<CharData<4>>(charddress);
        colorIndex = tile.sample(tileLoc);
        assert(colorIndex < 16);
        break;
    }
    case 8:
    {
        const auto &tile = _vram.readAs<CharData<8>>(charddress);
        colorIndex = tile.sample(tileLoc);
        break;
    }
    break;
    default:
        assert(false);
        break;
    }
    // sample tile color
    if (colorIndex != 0)
    {
        auto color = _palette[paletteSize * tile.color + colorIndex];
        return ColorWithPriority{color, _priorities[tile.bgPri ? 1 : 0]};
    }

    return std::nullopt;
}
