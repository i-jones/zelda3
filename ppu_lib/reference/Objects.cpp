#include "Objects.hpp"

#include <algorithm>

#include "CharData.hpp"

std::optional<ObjectRender::Ouput> ObjectRender::renderObjects(const OAM &oam, Vec2<int> pixel, ObjSel objectSelect, const VRAM &vram, const CGRam &cgRam)
{
    using PerObjOutput = std::optional<ObjectRender::Ouput>;
    std::array<std::optional<ObjectRender::Ouput>, OAM::NumObjects> perObjResult;
    for (int i = 0; i < perObjResult.size(); i++)
    {
        perObjResult[i] = renderObject(oam.get(i), pixel, objectSelect, vram, cgRam);
    }

    auto maxIt = std::max_element(
        perObjResult.begin(),
        perObjResult.end(),
        [](const PerObjOutput &a, PerObjOutput &b)
        {
            if (!a)
            {
                return true;
            }
            if (!b)
            {
                return false;
            }
            return a->priority < b->priority; });

    if (maxIt == perObjResult.end())
    {
        return std::nullopt;
    }
    return *maxIt;
}

std::optional<ObjectRender::Ouput> ObjectRender::renderObject(const ObjectData &obj, Vec2<int> pixel, ObjSel objectSelect, const VRAM &vram, const CGRam &cgRam)
{
    auto localPos = pixel - obj.position;

    const auto objectSize = ObjectRender::getObjectSize(objectSelect.getObjectSize(obj.type));
    const BBox<int> box(Vec2<int>::Zero(), objectSize);

    if (!box.intersects(localPos))
    {
        return std::nullopt;
    }

    if (obj.hFlip)
    {
        localPos.x() = objectSize.x() - 1 - localPos.x();
    }
    if (obj.vFlip)
    {
        localPos.y() = objectSize.y() - 1 - localPos.y();
    }

    const size_t baseAddr = objectSelect.nameBaseAddr << 13;
    const size_t addrOffset = (objectSelect.nameSelect + 1) << 12;

    size_t tileRowOffset = localPos.y() / 8;
    size_t tileColOffset = localPos.x() / 8;

    // tile Row and column of upper left 8x8 tile
    const int tileRow = (obj.name & 0x1F0) >> 4;
    const int tileColumn = obj.name & 0x0F;
    const int tileIndex = ((tileRow + tileRowOffset) << 4) | (tileColumn + tileColOffset);
    // assert(baseAddr == 0x4000);
    // assert((baseAddr + addrOffset) == 0x5000);
    const size_t charAddress = obj.index <= 0xff ? (baseAddr + tileIndex * 16) : (baseAddr + addrOffset + tileIndex * 16);
    const auto &tile = vram.readAs<CharData<4>>(charAddress);
    const auto colorIndex = tile.sample({localPos.x() % 8, localPos.y() % 8});
    if (colorIndex == 0)
    {
        return std::nullopt;
    }

    //  lookup color in pallete
    auto p = cgRam.getObjPallete(obj.color);
    Color c = p[colorIndex];
    return ObjectRender::Ouput{c, 100};
}
