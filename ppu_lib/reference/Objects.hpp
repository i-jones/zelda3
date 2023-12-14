#pragma once

#include <optional>

#include "../color.hpp"
#include "../vec.hpp"
#include "register.hpp"
#include "VRAM.hpp"

#pragma pack(push, 1)
struct ObjectDataPacked
{
    unsigned int hPosition : 8;
    unsigned int vPosition : 8;
    unsigned int name : 9;
    uint8_t color : 3;
    unsigned int objPriority : 2;
    bool hFlip : 1;
    bool vFlip : 1;
};
#pragma pack(pop)

static_assert(sizeof(ObjectDataPacked) == 4);

#pragma pack(push, 1)
struct ObjectDataExtra
{
    union
    {
        uint16_t raw;
        struct
        {
            unsigned int hPositionMSB0 : 1;
            ObjType size0 : 1;

            unsigned int hPositionMSB1 : 1;
            ObjType size1 : 1;

            unsigned int hPositionMSB2 : 1;
            ObjType size2 : 1;

            unsigned int hPositionMSB3 : 1;
            ObjType size3 : 1;

            unsigned int hPositionMSB4 : 1;
            ObjType size4 : 1;

            unsigned int hPositionMSB5 : 1;
            ObjType size5 : 1;

            unsigned int hPositionMSB6 : 1;
            ObjType size6 : 1;

            unsigned int hPositionMSB7 : 1;
            ObjType size7 : 1;
        };
    };

    std::pair<unsigned int, ObjType> get(size_t index) const
    {
        assert(index < 8);
        unsigned int msb = (raw >> (index * 2)) & 1;
        unsigned int type = (raw >> (index * 2 + 1)) & 1;
        return std::make_pair(msb, static_cast<ObjType>(type));
    }
};
#pragma pack(pop)
static_assert(sizeof(ObjectDataExtra) == 2);

struct ObjectData
{
    size_t index;
    Vec2<int> position;
    unsigned int name;
    uint8_t color;
    ObjType type;
    unsigned int objPriority;
    bool hFlip;
    bool vFlip;
};

#pragma pack(push, 1)
struct OAM
{
    static constexpr size_t NumObjects = 128;
    ObjectDataPacked obj[NumObjects];
    ObjectDataExtra extra[NumObjects / 8];

    ObjectData get(size_t index) const
    {
        const auto extraData = extra[index / 8].get(index % 8);
        const auto &packed = obj[index];
        int16_t x = (extraData.first ? 0xff : 0) << 8 | (packed.hPosition & 0xff);

        ObjectData result{index, {x, packed.vPosition}, packed.name, packed.color, extraData.second, packed.objPriority, packed.hFlip, packed.vFlip};
        return result;
    }
};

#pragma pack(pop)
static_assert(sizeof(OAM) == 544);

template <typename T>
class BBox
{
public:
    BBox(Vec2<T> upperLeft, Vec2<T> lowerRight) : _upperLeft(std::move(upperLeft)), _lowerRight(std::move(lowerRight)) {}

    bool intersects(Vec2<T> query) const
    {
        bool isect = true;
        for (int i = 0; i < 2; i++)
        {
            isect &= query[i] >= _upperLeft[i] && query[i] < _lowerRight[i];
        }
        return isect;
    }

private:
    Vec2<T> _upperLeft;
    Vec2<T> _lowerRight;
};

class ObjectRender
{
public:
    struct Ouput
    {
        Color color;
        int priority;
    };

    static Vec2<int> getObjectSize(ObjSize size)
    {
        int sizeInt = static_cast<int>(size);
        return Vec2<int>(sizeInt, sizeInt);
    }

    static std::optional<Ouput> renderObjects(const OAM &oam, Vec2<int> pixel, ObjSel objectSelect, const VRAM &vram);
    static std::optional<Ouput> renderObject(const ObjectData &obj, Vec2<int> pixel, ObjSel objectSelect, const VRAM &vram);
};