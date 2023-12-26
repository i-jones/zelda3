#include "Objects.hpp"

#include <algorithm>

#include "CharData.hpp"

std::optional<ObjectRender::Output> ObjectRender::renderObjects(
    const OAM &oam,
    Vec2<int> pixel,
    ObjSel objectSelect,
    const VRAM &vram,
    const CGRam &cgRam,
    const Priority::ObjPriorities &priorities)
{
    using PerObjOutput = std::optional<ObjectRender::Output>;
    std::optional<ObjectRender::Output> result;
    for (int i = 0; i < OAM::NumObjects; i++)
    {
        result = renderObject(oam.get(i), pixel, objectSelect, vram, cgRam, priorities);
        if (result)
        {
            break;
        }
    }
    return result;
}

std::optional<ObjectRender::Output> ObjectRender::renderObject(
    const ObjectData &obj,
    Vec2<int> pixel,
    ObjSel objectSelect,
    const VRAM &vram,
    const CGRam &cgRam,
    const Priority::ObjPriorities &priorities)
{
    auto localPos = pixel - obj.position;
    // wrap around
    localPos.y() = localPos.y() & 0xff;

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
    return ObjectRender::Output{c, priorities[obj.objPriority]};
}

void ObjectRender::getObjectsForScanline(
    const OAM &oam,
    int y,
    ObjSel objectSelect,
    std::vector<ObjectData> &output)
{
    output.clear();
    // TODO clip max number of objects per scanline
    output.reserve(OAM::NumObjects);
    for (int i = 0; i < OAM::NumObjects; i++)
    {
        auto obj = oam.get(i);
        auto localY = y - obj.position.y();
        // wrap around
        localY = localY & 0xff;
        const auto objectSize = ObjectRender::getObjectSize(objectSelect.getObjectSize(obj.type));
        if (localY >= 0 && localY < objectSize.y())
        {
            output.push_back(obj);
        }
    }
}

std::optional<ObjectRender::Output>
ObjectRender::renderObjectList(
    std::span<const ObjectData> objects,
    Vec2<int> pixel,
    ObjSel objectSelect,
    const VRAM &vram,
    const CGRam &cgRam,
    const Priority::ObjPriorities &priorities)
{
    using PerObjOutput = std::optional<ObjectRender::Output>;
    std::optional<ObjectRender::Output> result;
    for (const auto &obj : objects)
    {
        result = renderObject(obj, pixel, objectSelect, vram, cgRam, priorities);
        if (result)
        {
            break;
        }
    }
    return result;
}
