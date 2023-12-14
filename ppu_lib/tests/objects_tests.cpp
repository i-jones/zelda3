#include <gtest/gtest.h>

#include "ppu_lib/reference/Objects.hpp"

TEST(Objects, ObjectDataExtra)
{
    ObjectDataExtra data{};
    data.hPositionMSB0 = 0;
    data.size0 = ObjType::Small;
    data.hPositionMSB1 = 1;
    data.size1 = ObjType::Large;

    auto x = data.get(0);
    EXPECT_EQ(x.first, 0);
    EXPECT_EQ(x.second, ObjType::Small);

    x = data.get(1);

    EXPECT_EQ(x.first, 1);
    EXPECT_EQ(x.second, ObjType::Large);
}

TEST(Objects, OAM)
{
    OAM data{};
    int objIndex = 42;
    int extraIndex = objIndex / 8;

    auto &obj = data.obj[42];
    obj.hPosition = 1;
    obj.vPosition = 3;
    obj.name = 0xf0;
    obj.color = 5;
    obj.objPriority = 3;
    obj.hFlip = true;
    obj.vFlip = false;

    auto &extra = data.extra[extraIndex];
    extra.hPositionMSB2 = 0;
    extra.size2 = ObjType::Small;
    {
        const auto x = data.get(objIndex);
        EXPECT_EQ(x.position.x(), 1);
        EXPECT_EQ(x.position.y(), 3);
        EXPECT_EQ(x.name, 0xf0);
        EXPECT_EQ(x.color, 5);
        EXPECT_EQ(x.objPriority, 3);
        EXPECT_EQ(x.hFlip, true);
        EXPECT_EQ(x.vFlip, false);
        EXPECT_EQ(x.type, ObjType::Small);
    }

    extra.hPositionMSB2 = 1;
    extra.size2 = ObjType::Large;
    {
        const auto x = data.get(objIndex);
        EXPECT_EQ(x.position.x(), -255);
        EXPECT_EQ(x.position.y(), 3);
        EXPECT_EQ(x.name, 0xf0);
        EXPECT_EQ(x.color, 5);
        EXPECT_EQ(x.objPriority, 3);
        EXPECT_EQ(x.hFlip, true);
        EXPECT_EQ(x.vFlip, false);
        EXPECT_EQ(x.type, ObjType::Large);
    }
}