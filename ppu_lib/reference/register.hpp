#pragma once

#include <array>
#include <cstdint>
#include <type_traits>

enum class ForcedBlanking : unsigned char
{
    NonBlanking = 0,
    Blanking = 1
};

template <typename T>
struct Write
{
    void write(std::uint8_t value)
    {
        static_assert(sizeof(T) == 1);
        static_assert(std::is_pod_v<T>);
        *reinterpret_cast<std::uint8_t *>(this) = value;
    }
};

#define REGISTER_CONSTRAINT(RegisterType)     \
    static_assert(sizeof(RegisterType) == 1); \
    static_assert(std::is_pod_v<RegisterType>);

/*
ADDRESS: 2100H
NAME: INIDISP
CONTENTS: INITIAL SETTINGS FOR SCREEN
*/

#pragma pack(push, 1)
struct IniDisp : public Write<IniDisp>
{
    unsigned int fade : 4; // Screen Brightness. 15 Bright, 0 Dark.
    unsigned int _reserved : 3;
    ForcedBlanking blanking : 1;
};
#pragma pack(pop)
REGISTER_CONSTRAINT(IniDisp);

/*
ADDRESS: 2101H
NAME: OBJSEL
CONTENTS: INITIAL SETTINGS FOR SCREEN
*/

enum class ObjSize
{
    Size8 = 8,
    Size16 = 16,
    Size32 = 32,
    Size64 = 64,
};

enum class ObjType
{
    Small = 0,
    Large = 1,
};

#pragma pack(push, 1)
struct ObjSel : public Write<ObjSel>
{
    /* OBJECT BASE ADDRESS (UPPER З BIT)
      Designate the segment (8K-word/ segment) address which the OBJ data is stored in the VRAM.
      (See pages A-1 and A-2)
    */
    unsigned int nameBaseAddr : 3;
    /*
    BJECT DATA AREASELECT
    The upper 4K-word out of the area (8K-word) desianated by "ObjectBase Address" is assigned as
    the Base Area, and the area of the lower 4K-wordcombined with its Base Area can be selected.
    (See pages A-1 and A-2)
    */
    unsigned int nameSelect : 2;
    unsigned int sizeSelect : 3;

    ObjSize getObjectSize(ObjType type)
    {
        constexpr std::array<ObjSize, 6> smallSizes = {
            ObjSize::Size8, ObjSize::Size8, ObjSize::Size8,
            ObjSize::Size16, ObjSize::Size16, ObjSize::Size32};
        constexpr std::array<ObjSize, 6> largeSizes = {
            ObjSize::Size16, ObjSize::Size32, ObjSize::Size64,
            ObjSize::Size32, ObjSize::Size64, ObjSize::Size64};
        auto index = sizeSelect;
        if (index >= smallSizes.size())
        {
            std::abort();
        }
        return type == ObjType::Small ? smallSizes[index] : largeSizes[index];
    }
};
#pragma pack(pop)
REGISTER_CONSTRAINT(ObjSel);

/*
ADDRESS: 2102H /2103H
NAME:OAMADDL / OAMADDH
CONTENTS: ADDRESS FOR ACCESSING OAM (OBJECT ATTRIBUTE MEMORY)
*/
#pragma pack(push, 1)
struct OamAddL : public Write<OamAddL>
{
    uint8_t oamAddress;
};
#pragma pack(pop)
REGISTER_CONSTRAINT(OamAddL);

#pragma pack(push, 1)
struct OamAddH : public Write<OamAddH>
{
    unsigned int oamAddressMSB : 1;
    unsigned int _reserved : 6;
    bool oamPriorityRotation : 1;
};
#pragma pack(pop)
REGISTER_CONSTRAINT(OamAddH);

/*
ADDRESS: 2105H
NAME: BG MODE
CONTENTS: BG MODE & CHARACTER SIZE SETTINGS
*/
enum class BgMode : unsigned char
{
    Mode0 = 0,
    Mode1 = 1,
    Mode2,
    Mode3,
    Mode4,
    Mode5,
    Mode6,
    Mode7,
};

enum class BgSize : unsigned char
{
    Size8x8 = 0,
    Size16x16 = 1,

};

#pragma pack(push, 1)
struct BgModeReg : public Write<BgModeReg>
{
    BgMode bgMode : 3;
    // Make BG3 highest priority during BG Mode 0 or 1
    bool bg3Priority : 1;
    BgSize bg1Size : 1;
    BgSize bg2Size : 1;
    BgSize bg3Size : 1;
    BgSize bg4Size : 1;
};
#pragma pack(pop)
REGISTER_CONSTRAINT(BgModeReg);

/*
ADDRESS: 2106H
NAME: MOSAIC
CONTENTS: SIZE &SCREEN DESIGNATION FOR MOSAIC DISPLAY
*/

enum class MosaicState : unsigned char
{
    Off = 0,
    On = 1
};

#pragma pack(push, 1)
struct Mosaic : public Write<Mosaic>
{
    MosaicState bg1 : 1;
    MosaicState bg2 : 1;
    MosaicState bg3 : 1;
    MosaicState bg4 : 1;
    unsigned int mosaicSize : 4;
};
#pragma pack(pop)
REGISTER_CONSTRAINT(BgModeReg);

/*
ADDRESS: 2107H /2108H /2109H / 210AH
NAME: BG1SC / BG2SC / BG3SC / BGASC
CONTENTS: ADDRESS FOR STORING SC-DATA OF EACH B G& SC SIZE DESIGNATION (MODE 0 ~ 6)
*/

enum class BgScreenSize : unsigned char
{
    ScreenSize00 = 0,
    ScreenSize01 = 1,
    ScreenSize10 = 2,
    ScreenSize11 = 3,
};
#pragma pack(push, 1)
struct BGSC : public Write<BGSC>
{
    // SCREEN SIZE DESIGNATION (pages A-21 and A-22) Designate BackgroundScreen Size
    BgScreenSize scSize : 2;
    // CKGROUND SCREEN BASE ADDRESS (UPPER 6-BIT)
    // Designate the segment in which BG-SC data in the VRAM is stored. (1K-WORD/SEGMENT)
    unsigned int scBaseAddress : 6;
};
#pragma pack(pop)
REGISTER_CONSTRAINT(BGSC);

struct BgScreenSizes
{
    BGSC bg[4];
};

/*
ADDRESS: 210DH / 210EH
NAME: BG1HOFS / BG1VOFS
CONTENTS: H/V SCROLL VALUE DESIGNATION FOR BG-1
*/
class BG1OffsetRegiser
{
public:
    void reset()
    {
        _writeIndex = 0;
    }
    void write(std::uint8_t value)
    {
        _bytes[_writeIndex] = value;
        _writeIndex = (_writeIndex + 1) % 2;
    }

    unsigned int offset() const
    {
        return _offset;
    }
    int mode7Offset() const
    {
        return _mode7Offset;
    }

private:
    union
    {
        std::uint8_t _bytes[2];
        unsigned int _offset : 10;
        int _mode7Offset : 13;
    };
    unsigned int _writeIndex = 0;
};

/*
ADDRESS: 210FH /2110 H/2111 H/2112 H/2113 H/2114 H
NAME: BG2H0FS / BG2VOFS / BG3HOFS /BG3VOFS / BG4H0FS / BG4VOFS
CONTENTS: H / SCROLL VALUE DESIGNATION FOR BG-2, 3, 4
*/
class BGOffsetRegiser
{
public:
    void reset()
    {
        _writeIndex = 0;
    }
    void write(std::uint8_t value)
    {
        _bytes[_writeIndex] = value;
        _writeIndex = (_writeIndex + 1) % 2;
    }

    unsigned int offset() const
    {
        return _offset;
    }

private:
    union
    {
        std::uint8_t _bytes[2];
        unsigned int _offset : 10;
    };
    unsigned int _writeIndex = 0;
};

/*
ADDRESS: 2115H
NAME: VMAINC
CONTENTS: VRAM ADDRESS INCREMENT VALUE DESIGNATION
*/
#pragma pack(push, 1)
struct VMAINC : public Write<VMAINC>
{
    // TODO
};
#pragma pack(pop)
REGISTER_CONSTRAINT(VMAINC);

/*
ADDRESS: 211AH
NAME: M7SEL
CONTENTS: INITIAL SETTING IN SCREEN MODE-7
*/
enum class Mode7ScreenOver : unsigned char
{
    Repeat = 0,
    UNKNOWN = 1,
    BackDrop = 2,
    RpeateChar0 = 3,
};

#pragma pack(push, 1)
struct M7Sel : public Write<M7Sel>
{
    bool hFlip : 1;
    bool vFlip : 1;
    int _reserved : 4;
    Mode7ScreenOver screenOver : 2;
};
#pragma pack(pop)
REGISTER_CONSTRAINT(M7Sel);
