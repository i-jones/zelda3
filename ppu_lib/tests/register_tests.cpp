#include <gtest/gtest.h>

#include "ppu_lib/PPUBase.hpp"

#include "ppu_lib/reference/register.hpp"

// Demonstrate some basic assertions.
TEST(HelloTest, BasicAssertions)
{
    // Expect two strings not to be equal.
    EXPECT_STRNE("hello", "world");
    // Expect equality.
    EXPECT_EQ(7 * 6, 42);
}

TEST(Registers, IniDispTest)
{
    IniDisp x{};
    x.write(0xff);
    EXPECT_EQ(x.fade, 15);
    EXPECT_EQ(x.blanking, ForcedBlanking::Blanking);

    x.write(2);

    EXPECT_EQ(x.fade, 2);
    EXPECT_EQ(x.blanking, ForcedBlanking::NonBlanking);
}

TEST(Registers, ObjSel)
{
    ObjSel x{};
    x.write(0xff);
    EXPECT_EQ(x.nameBaseAddr, 7);
    EXPECT_EQ(x.nameSelect, 3);

    x.write(2);
    EXPECT_EQ(x.nameBaseAddr, 2);
    EXPECT_EQ(x.nameSelect, 0);
    EXPECT_EQ(x.getObjectSize(ObjType::Small), ObjSize::Size8);
    EXPECT_EQ(x.getObjectSize(ObjType::Large), ObjSize::Size16);
}

TEST(RegistersDeathTest, ObjSel)
{
    ObjSel x{};
    x.write(0xff);
    EXPECT_DEATH(x.getObjectSize(ObjType::Small), "");
}

TEST(Registers, BgModeReg)
{
    BgModeReg x{};
    x.write(0xff);
    EXPECT_EQ(x.bgMode, BgMode::Mode7);
    EXPECT_EQ(x.bg3Priority, true);
    EXPECT_EQ(x.bg1Size, BgSize::Size16x16);
    EXPECT_EQ(x.bg2Size, BgSize::Size16x16);
    EXPECT_EQ(x.bg3Size, BgSize::Size16x16);
    EXPECT_EQ(x.bg4Size, BgSize::Size16x16);

    x.write(0b10101010);
    EXPECT_EQ(x.bgMode, BgMode::Mode2);
    EXPECT_EQ(x.bg3Priority, true);
    EXPECT_EQ(x.bg1Size, BgSize::Size8x8);
    EXPECT_EQ(x.bg2Size, BgSize::Size16x16);
    EXPECT_EQ(x.bg3Size, BgSize::Size8x8);
    EXPECT_EQ(x.bg4Size, BgSize::Size16x16);
}

TEST(Registers, Mosaic)
{
    Mosaic x{};
    x.write(0xff);
    EXPECT_EQ(x.bg1, MosaicState::On);
    EXPECT_EQ(x.bg2, MosaicState::On);
    EXPECT_EQ(x.bg3, MosaicState::On);
    EXPECT_EQ(x.bg4, MosaicState::On);
    EXPECT_EQ(x.mosaicSize, 15);

    x.write(0b10101010);
    EXPECT_EQ(x.bg1, MosaicState::Off);
    EXPECT_EQ(x.bg2, MosaicState::On);
    EXPECT_EQ(x.bg3, MosaicState::Off);
    EXPECT_EQ(x.bg4, MosaicState::On);
    EXPECT_EQ(x.mosaicSize, 10);
}

TEST(Registers, Offset)
{
    BGOffsetRegiser r{};

    r.reset();
    r.write(0xff);
    r.write(0xff);

    EXPECT_EQ(r.offset(), 0x3ff);

    r.write(0x0f);
    r.write(0xf0);
    EXPECT_EQ(r.offset(), 0xf);
}

TEST(Registers, BG1Offset)
{
    BG1OffsetRegiser r{};

    r.reset();
    r.write(0xff);
    r.write(0xff);

    EXPECT_EQ(r.offset(), 1023);
    EXPECT_EQ(r.mode7Offset(), -1);

    r.write(0xff);
    r.write(0x0f);
    EXPECT_EQ(r.offset(), 1023);
    EXPECT_EQ(r.mode7Offset(), 4095);
    r.write(0x00);
    r.write(0x10);
    EXPECT_EQ(r.offset(), 0);
    EXPECT_EQ(r.mode7Offset(), -4096);
}