#pragma once

#include <span>

#include "register.hpp"
#include "../vec.hpp"

#pragma pack(push, 1)
struct BgScData
{
    unsigned int name : 9;
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
    BgBaseScreen(const BgScData *base) : _data(base, Width * Height){};
    BgScData sample(Vec2<int> loc) const;

private:
    std::span<const BgScData> _data;
};

class BgScreen
{
public:
    BgScreen(BGSC settings) {}
    BgScData sample(Vec2<int> loc) const;

private:
};
