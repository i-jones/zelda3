#include <gtest/gtest.h>

#include "ppu_lib/reference/Fixed.hpp"

/*
TEST(Fixed, Conv)
{
    auto f = Fixed<int, 8>::fromFloat(0.5);
    EXPECT_EQ(f.toFloat(), 0.5);
    EXPECT_EQ(f.raw(), 0x80);
    EXPECT_EQ(f.toInt(), 0);

    f = Fixed<int, 8>::fromFloat(-0.5);
    EXPECT_EQ(f.toFloat(), -0.5);
    EXPECT_EQ(f.raw(), ~0x80 + 1);
    EXPECT_EQ(f.toInt(), -1);

    f = Fixed<int, 8>::fromFloat(1.0 / 512);
    EXPECT_EQ(f.toFloat(), 0);
    EXPECT_EQ(f.raw(), 0);
    EXPECT_EQ(f.toInt(), 0);

    f = Fixed<int, 8>::fromFloat(1.5);
    EXPECT_EQ(f.toFloat(), 1.5);
    EXPECT_EQ(f.raw(), 0x180);
    EXPECT_EQ(f.toInt(), 1);
}

TEST(Fixed, AddSub)
{
    {
        auto f = Fixed<int, 8>::fromFloat(0.5);
        auto f2 = Fixed<int, 8>::fromFloat(1.25);

        EXPECT_EQ((f + f).toFloat(), 1.0);
        EXPECT_EQ((f + f2).toFloat(), 1.75);

        EXPECT_EQ((f - f).toFloat(), 0.0);
        EXPECT_EQ((f - f2).toFloat(), -0.75);
    }
    {
        auto f = Fixed<std::int16_t, 8>::fromFloat(0.5);
        auto f2 = Fixed<std::int16_t, 8>::fromFloat(1.25);

        EXPECT_EQ((f + f).toFloat(), 1.0);
        EXPECT_EQ((f + f2).toFloat(), 1.75);

        EXPECT_EQ((f - f).toFloat(), 0.0);
        EXPECT_EQ((f - f2).toFloat(), -0.75);
    }
    {
        auto f = Fixed<std::int16_t, 8>::fromFloat(127);
        auto f2 = Fixed<std::int16_t, 8>::fromFloat(-127);

        EXPECT_EQ((f + f).toFloat(), 254.0);
        EXPECT_EQ((f + f2).toFloat(), 0.0);

        EXPECT_EQ((f - f).toFloat(), 0.0);
        EXPECT_EQ((f - f2).toFloat(), 254.0);
    }
    {
        auto f = Fixed<std::int16_t, 8>::fromFloat(127);
        EXPECT_EQ((128 - f).toFloat(), 1.0);
        EXPECT_EQ((-127 + f).toFloat(), 0.0);
    }
    {
        auto f = Fixed<std::int16_t, 8>::fromFloat(127);
        auto f2 = Fixed<int, 16>::fromFloat(-127);

        EXPECT_EQ((f + f2).toFloat(), 0.0);
        EXPECT_EQ((f - f2).toFloat(), 254.0);
    }
}

TEST(Fixed, Mul)
{
    {
        auto f = Fixed<int, 8>::fromFloat(0.5);
        auto f2 = Fixed<int, 8>::fromFloat(1.25);

        EXPECT_EQ((f * f).toFloat(), 0.25);
        EXPECT_EQ((f * f2).toFloat(), 1.25 / 2);
    }
    {
        auto f = Fixed<std::int16_t, 8>::fromFloat(127.5);
        auto f2 = Fixed<std::int16_t, 8>::fromFloat(-127.5);

        EXPECT_EQ((f * f).toFloat(), 16256.25);
        EXPECT_EQ((f * f2).toFloat(), -16256.25);
    }
    {
        double smallest = 1.0 / 256;
        auto df = 127 + smallest;
        auto df2 = -127 - smallest;
        auto f = Fixed<std::int16_t, 8>::fromFloat(df);
        auto f2 = Fixed<std::int16_t, 8>::fromFloat(df2);

        EXPECT_EQ((f * f).toFloat(), df * df);
        EXPECT_EQ((f * f2).toFloat(), df2 * df);
        EXPECT_EQ((f2 * f2).toFloat(), df2 * df2);
    }
}
*/