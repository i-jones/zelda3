#include <gtest/gtest.h>

#include "ppu_lib/reference/CharData.hpp"

TEST(CharData, BPP2)
{
    CharData<2> data;

    data.bitplaneData[0][0].low = 0b01000001;
    data.bitplaneData[0][0].high = 0b00000000;
    data.bitplaneData[0][1].low = 0b01000001;
    data.bitplaneData[0][1].high = 0b11000011;

    data.bitplaneData[0][6].low = 0b01000001;
    data.bitplaneData[0][6].high = 0b00000000;
    data.bitplaneData[0][7].low = 0b01000001;
    data.bitplaneData[0][7].high = 0b11000011;

    EXPECT_EQ(data.sample({0, 0}), 0);
    EXPECT_EQ(data.sample({1, 0}), 1);
    EXPECT_EQ(data.sample({0, 1}), 2);
    EXPECT_EQ(data.sample({1, 1}), 3);

    EXPECT_EQ(data.sample({6, 0}), 0);
    EXPECT_EQ(data.sample({7, 0}), 1);
    EXPECT_EQ(data.sample({6, 1}), 2);
    EXPECT_EQ(data.sample({7, 1}), 3);

    EXPECT_EQ(data.sample({0, 6}), 0);
    EXPECT_EQ(data.sample({1, 6}), 1);
    EXPECT_EQ(data.sample({0, 7}), 2);
    EXPECT_EQ(data.sample({1, 7}), 3);

    EXPECT_EQ(data.sample({6, 6}), 0);
    EXPECT_EQ(data.sample({7, 6}), 1);
    EXPECT_EQ(data.sample({6, 7}), 2);
    EXPECT_EQ(data.sample({7, 7}), 3);
}

TEST(CharData, BPP4)
{
    CharData<4> data{};

    data.bitplaneData[0][0].low = 0b01010101;
    data.bitplaneData[0][0].high = 0b00110011;
    data.bitplaneData[1][0].low = 0b00001111;
    data.bitplaneData[1][0].high = 0b00000000;

    data.bitplaneData[0][1].low = 0b01010101;
    data.bitplaneData[0][1].high = 0b00110011;
    data.bitplaneData[1][1].low = 0b00001111;
    data.bitplaneData[1][1].high = 0b11111111;

    for (int x = 0; x < 7; x++)
    {
        EXPECT_EQ(data.sample({x, 0}), x);
        EXPECT_EQ(data.sample({x, 1}), x + 8);
    }
}

// TODO BPP8