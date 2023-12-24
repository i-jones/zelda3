#pragma once

#include <array>
#include <cstdint>
#include <type_traits>

enum LayerIndex
{
    kLayerIndexBG1 = 0,
    kLayerIndexBG2,
    kLayerIndexBG3,
    kLayerIndexBG4,
    kLayerIndexObj,
    kLayerIndexBack,
    kLayerIndexCount
};

enum LayerMaskFlags : unsigned int
{
    kLayerMaskNone = 0,
    kLayerMaskBg1 = 1 << kLayerIndexBG1,
    kLayerMaskBg2 = 1 << kLayerIndexBG2,
    kLayerMaskBg3 = 1 << kLayerIndexBG3,
    kLayerMaskBg4 = 1 << kLayerIndexBG4,
    kLayerMaskObj = 1 << kLayerIndexObj,
    kLayerMaskBack = 1 << kLayerIndexBack,
};

constexpr uint8_t RegisterIndex(uint16_t snesAddr)
{
    return snesAddr - 0x2100;
}

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
    /*
      OBJECT BASE ADDRESS (UPPER З BIT)
      Designate the segment (8K-word/ segment) address which the OBJ data is stored in the VRAM.
      (See pages A-1 and A-2)
    */
    unsigned int nameBaseAddr : 3;
    /*
    OBJECT DATA AREASELECT
    The upper 4K-word out of the area (8K-word) designated by "ObjectBase Address" is assigned as
    the Base Area, and the area of the lower 4K-wordcombined with its Base Area can be selected.
    (See pages A-1 and A-2)
    */
    unsigned int nameSelect : 2;
    unsigned int sizeSelect : 3;

    ObjSize getObjectSize(ObjType type) const
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
ADDRESS: 210BH/210CH
NAME: BG12NBA / BG34NBA
CONTENTS: BG CHARACTER DATA AREA DESIGNATION
*/
#pragma pack(push, 1)
struct BGCharacterDataArea : public Write<BGCharacterDataArea>
{
    unsigned int baseAddr0 : 4;
    unsigned int baseAddr1 : 4;
};
#pragma pack(pop)
REGISTER_CONSTRAINT(BGCharacterDataArea);

struct BGNameAddresses
{
    BGCharacterDataArea bg12;
    BGCharacterDataArea bg34;

    // TODO add convenience accessors
};

/*
ADDRESS: 210DH / 210EH
NAME: BG1HOFS / BG1VOFS
CONTENTS: H/V SCROLL VALUE DESIGNATION FOR BG-1
*/
class BG1OffsetRegister
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

struct BG1Offsets
{
    BG1OffsetRegister hOffset;
    BG1OffsetRegister vOffset;
};

/*
ADDRESS: 210FH /2110 H/2111 H/2112 H/2113 H/2114 H
NAME: BG2H0FS / BG2VOFS / BG3HOFS /BG3VOFS / BG4H0FS / BG4VOFS
CONTENTS: H / SCROLL VALUE DESIGNATION FOR BG-2, 3, 4
*/
class BGOffsetRegister
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

struct BGOffsets
{
    BGOffsetRegister hOffset;
    BGOffsetRegister vOffset;
};

struct BGsOffsets
{
    BGOffsets bg[3];
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

/*
ADDRESS: 2123H / 2124H / 2125H
NAME: W12SEL/ W34SEL WOBJSEL
CONTENTS: WINDOW MASK SETTINGS (BG1~BG4, OBJ, COLOR)
*/

enum class WindowInOut : unsigned char
{
    In = 0,
    Out = 1,
};

enum class WindowEnabled : unsigned char
{
    Off = 0,
    On = 1,
};

#pragma pack(push, 1)
struct WindowMaskSettings : public Write<WindowMaskSettings>
{
    WindowInOut bg1InOut1 : 1;
    WindowEnabled bg1Enable1 : 1;
    WindowInOut bg1InOut2 : 1;
    WindowEnabled bg1Enable2 : 1;
    WindowInOut bg2InOut1 : 1;
    WindowEnabled bg2Enable1 : 1;
    WindowInOut bg2InOut2 : 1;
    WindowEnabled bg2Enable2 : 1;
};
#pragma pack(pop)
REGISTER_CONSTRAINT(WindowMaskSettings);

struct WindowSettings
{
    WindowMaskSettings windowBg12;
    WindowMaskSettings windowBg34;
    WindowMaskSettings windowObjColor;
};

/*
ADDRESS: 212AH/ 212BH
NAME: WBGLOG/ WOBJLOGI
CONTENTS: MASK LOGIC SETTINGS FOR WINDOW-1 & 2 ON EACH SCREEN
*/

enum class WindowLogic : unsigned char
{
    OR = 0,
    AND = 1,
    XOR = 2,
    XNOR = 3,
};

#pragma pack(push, 1)
struct WindowBGLogic : public Write<WindowBGLogic>
{
    WindowLogic bg1 : 2;
    WindowLogic bg2 : 2;
    WindowLogic bg3 : 2;
    WindowLogic bg4 : 2;
};
#pragma pack(pop)
REGISTER_CONSTRAINT(WindowBGLogic);

#pragma pack(push, 1)
struct WindowObjectLogic : public Write<WindowObjectLogic>
{
    WindowLogic obj : 2;
    WindowLogic color : 2;
    unsigned int _reserved : 4;
};
#pragma pack(pop)
REGISTER_CONSTRAINT(WindowObjectLogic);

enum class LayerFlag : bool
{
    Disable = 0,
    Enable = 1
};

#pragma pack(push, 1)
struct LayerFlags : public Write<LayerFlags>
{
    LayerFlag bg1 : 1;
    LayerFlag bg2 : 1;
    LayerFlag bg3 : 1;
    LayerFlag bg4 : 1;
    LayerFlag obj : 1;
    unsigned int _reserved : 3;

    LayerFlag operator[](LayerIndex idx) const
    {
        switch (idx)
        {
        case LayerIndex::kLayerIndexBG1:
            return bg1;
        case LayerIndex::kLayerIndexBG2:
            return bg2;
        case LayerIndex::kLayerIndexBG3:
            return bg3;
        case LayerIndex::kLayerIndexBG4:
            return bg4;
        case LayerIndex::kLayerIndexObj:
            return obj;
        default:
            assert(false);
            return LayerFlag::Disable;
        }
    }
};
#pragma pack(pop)
REGISTER_CONSTRAINT(LayerFlags);

/*
ADDRESS: 2130H
NAME: CGSWSEL
CONTENTS: INITIAL SETTINGS FOR FIXEDC O L O R ADDITION OR SCREEN ADDITION
*/

enum class ColorWindowFunction : unsigned char
{
    On = 0,
    InsideWindow = 1,
    OutsideWindow = 2,
    Off = 3
};

enum class ColorWindowSource : unsigned char
{
    FixedColor = 0,
    Subscreen = 1,
};

#pragma pack(push, 1)
struct ColorWindowSelect : public Write<ColorWindowSelect>
{
    bool directSelect : 1;
    ColorWindowSource ccAddEnable : 1;
    unsigned int _reserved : 2;
    ColorWindowFunction sub : 2;
    ColorWindowFunction main : 2;
};
#pragma pack(pop)
REGISTER_CONSTRAINT(ColorWindowSelect);

/*
ADDRESS: 2131H
NAME: CGADSUB
CONTENTS: ADDITION/SUBTRACTION & SUBTRACTION DESIGNATION FOR EACH BG SCREEN OBJ & BACKGROUND COLOR
*/

enum class ColorMode : unsigned char
{
    Addition = 0,
    Subtraction = 1
};

#pragma pack(push, 1)
struct ColorAddSub : public Write<ColorAddSub>
{
    unsigned int enableMask : 6;
    bool halfEnable : 1;
    ColorMode addSub : 1;
};
#pragma pack(pop)
REGISTER_CONSTRAINT(ColorAddSub);

/*
ADDRESS: 2132H
NAME: COLDATA
CONTENTS: FIXED COLOR DATA FOR FIXED COLOR ADDITION/SUBTRACTION
*/
#pragma pack(push, 1)
struct ColorData : public Write<ColorData>
{
    unsigned int colorBrillianceData : 5;
    bool red : 1;
    bool green : 1;
    bool blue : 1;
};
#pragma pack(pop)
REGISTER_CONSTRAINT(ColorData);

/*
ADDRESS: 2133H
NAME: SETINI
CONTENTS: SCREEN INITIALSETTING
*/

enum class InterlaceMode : unsigned char
{
    NonInterlace = 0,
    ScanningInterlace = 1,
};

enum class VDisplay : unsigned char
{
    Display224 = 0,
    Display239 = 1,
};

#pragma pack(push, 1)
struct ScreenInitSettings : public Write<ScreenInitSettings>
{
    InterlaceMode interlace : 1;
    bool objVSelect : 1;
    VDisplay bgVSelect : 1;
    unsigned int _reserved : 2;
    bool extBGMode : 1;
    bool externalSynch : 1;
};
#pragma pack(pop)
REGISTER_CONSTRAINT(ScreenInitSettings);
