#include "SNESPPU.hpp"

#include "color.hpp"
#include "reference/register.hpp"

SNESPPU::SNESPPU() : _ppu(ppu_init(), ppu_free)
{
    vram = &_ppu->vram;
}

SNESPPU::~SNESPPU() = default;

void SNESPPU::reset()
{
    ppu_reset(_ppu.get());
}

void SNESPPU::runLine(int line)
{
    ppu_runLine(_ppu.get(), line);
}

uint8_t SNESPPU::read(uint8_t adr)
{
    return ppu_read(_ppu.get(), adr);
}

void SNESPPU::write(uint8_t adr, uint8_t val)
{
    ppu_write(_ppu.get(), adr, val);
}

void SNESPPU::writeCGRam(const void *data, size_t size)
{
    memcpy(_ppu->cgram, data, size);
}

void SNESPPU::writeOam(const void *data, size_t size)
{
    memcpy(_ppu->oam, data, size);
}

void SNESPPU::saveLoad(PpuSaveLoadFunc *func, void *context)
{
    ppu_saveload(_ppu.get(), func, context);
}

void SNESPPU::beginDrawing(uint8_t *buffer, size_t pitch, uint32_t render_flags)
{
    PpuBeginDrawing(_ppu.get(), buffer, pitch, render_flags);
}

int SNESPPU::getCurrentRenderScale(uint32_t render_flags)
{
    return PpuGetCurrentRenderScale(_ppu.get(), render_flags);
}

void SNESPPU::setMode7PerspectiveCorrection(int low, int high)
{
    PpuSetMode7PerspectiveCorrection(_ppu.get(), low, high);
}

void SNESPPU::setExtraSideSpace(int left, int right, int bottom)
{
    PpuSetExtraSideSpace(_ppu.get(), left, right, bottom);
}

uint8_t SNESPPU::getMode()
{
    return _ppu->mode;
}

uint8_t SNESPPU::getExtraLeftRight()
{
    return _ppu->extraLeftRight;
}

void SNESPPU::setExtraLeftRight(uint8_t extraLeftRight)
{
    _ppu->extraLeftRight = extraLeftRight;
}

Ppu *SNESPPU::getPpu()
{
    return _ppu.get();
}

flatbuffers::Offset<PPUCommand::PPUState>
SNESPPU::createState(flatbuffers::FlatBufferBuilder &fbbuilder)
{

    auto cgram = fbbuilder.CreateVector(
        reinterpret_cast<uint8_t *>(_ppu->cgram), sizeof(_ppu->cgram));

    auto vram = fbbuilder.CreateVector(
        reinterpret_cast<uint8_t *>(_ppu->vram), sizeof(_ppu->vram));

    auto oam = fbbuilder.CreateVector(
        reinterpret_cast<uint8_t *>(_ppu->oam), sizeof(_ppu->oam));

    PPUCommand::PPUStateBuilder builder(fbbuilder);
    builder.add_cgram(cgram);
    builder.add_vram(vram);
    builder.add_oam(oam);

    {
        IniDisp inidisp;
        inidisp.blanking = _ppu->forcedBlank ? ForcedBlanking::Blanking : ForcedBlanking::NonBlanking;
        inidisp.fade = _ppu->brightness;
        builder.add_inidisp(inidisp.raw());
    }
    {
        ObjSel objsel;
        objsel.write(2);
        assert(objsel.raw() == 2);
        builder.add_objsel(objsel.raw());
    }

    builder.add_oamaddl(_ppu->oamAdr & 0xff);
    builder.add_oamaddh((_ppu->oamAdr >> 8) & 0xff);

    {
        BgModeReg bgmode;
        bgmode.write(0);
        bgmode.bgMode = static_cast<BgMode>(_ppu->mode);
        bgmode.bg3Priority = _ppu->mode == 7 ? false : true;
        assert(bgmode.raw() == 7 || bgmode.raw() == 9);
        builder.add_bgmode(bgmode.raw());
    }
    {
        Mosaic mosaic;
        mosaic.mosaicSize = _ppu->mosaicSize - 1;
        mosaic.bg1 = (_ppu->mosaicEnabled & 1) ? MosaicState::On : MosaicState::Off;
        mosaic.bg2 = (_ppu->mosaicEnabled & 2) ? MosaicState::On : MosaicState::Off;
        mosaic.bg3 = (_ppu->mosaicEnabled & 4) ? MosaicState::On : MosaicState::Off;
        mosaic.bg4 = (_ppu->mosaicEnabled & 8) ? MosaicState::On : MosaicState::Off;
        builder.add_mosaic(mosaic.raw());
    }
    {
        BGSC bgsc[4];
        for (int i = 0; i < 4; i++)
        {
            int size = (_ppu->bgLayer[i].tilemapWider ? 1 : 0) | ((_ppu->bgLayer[i].tilemapHigher ? 1 : 0) << 1);
            bgsc[i].scSize = static_cast<BgScreenSize>(size);
            bgsc[i].scBaseAddress = _ppu->bgLayer[i].tilemapAdr >> 10;
        }
        builder.add_bg1sc(bgsc[0].raw());
        builder.add_bg2sc(bgsc[1].raw());
        builder.add_bg3sc(bgsc[2].raw());
        builder.add_bg4sc(bgsc[3].raw());
    }
    {
        BGCharacterDataArea bg12nba;
        bg12nba.baseAddr0 = _ppu->bgLayer[0].tileAdr >> 12;
        bg12nba.baseAddr1 = _ppu->bgLayer[1].tileAdr >> 12;
        builder.add_bg12nba(bg12nba.raw());
    }

    {
        BGCharacterDataArea bg34nba;
        bg34nba.baseAddr0 = _ppu->bgLayer[2].tileAdr >> 12;
        bg34nba.baseAddr1 = _ppu->bgLayer[3].tileAdr >> 12;
        builder.add_bg34nba(bg34nba.raw());
    }
    {
        builder.add_bg1hofs(_ppu->bgLayer[0].hScroll);
        builder.add_bg1hofsmode7(_ppu->m7matrix[6]);
        builder.add_bg1vofs(_ppu->bgLayer[0].vScroll);
        builder.add_bg1vofsmode7(_ppu->m7matrix[7]);

        builder.add_bg2hofs(_ppu->bgLayer[1].hScroll);
        builder.add_bg2vofs(_ppu->bgLayer[1].vScroll);

        builder.add_bg3hofs(_ppu->bgLayer[2].hScroll);
        builder.add_bg3vofs(_ppu->bgLayer[2].vScroll);

        builder.add_bg4hofs(_ppu->bgLayer[3].hScroll);
        builder.add_bg4vofs(_ppu->bgLayer[3].vScroll);
    }

    {
        uint8_t vmainc = _ppu->vramIncrementOnHigh ? 0x80 : 0;
        if (_ppu->vramIncrement == 1)
        {
        }
        else if (_ppu->vramIncrement == 32)
        {
            vmainc |= 1;
        }
        else
        {
            vmainc |= 3;
        }
        builder.add_vmainc(vmainc);
    }
    builder.add_vmadd(_ppu->vramPointer);

    {
        M7Sel m7sel;
        m7sel.write(0);
        m7sel.hFlip = _ppu->m7xFlip;
        m7sel.vFlip = _ppu->m7yFlip;
        if (_ppu->m7largeField)
        {
            if (_ppu->m7charFill)
            {
                m7sel.screenOver = Mode7ScreenOver::RpeateChar0;
            }
            else
            {
                m7sel.screenOver = Mode7ScreenOver::BackDrop;
            }
        }
        else
        {
            if (!_ppu->m7charFill)
            {
                m7sel.screenOver = Mode7ScreenOver::Repeat;
            }
        }

        assert(m7sel.raw() == 0x80);
        builder.add_m7sel(m7sel.raw());

        builder.add_m7a(_ppu->m7matrix[0]);
        builder.add_m7b(_ppu->m7matrix[1]);
        builder.add_m7c(_ppu->m7matrix[2]);
        builder.add_m7d(_ppu->m7matrix[3]);
        builder.add_m7x(_ppu->m7matrix[4]);
        builder.add_m7y(_ppu->m7matrix[5]);
    }

    builder.add_cgadd(_ppu->cgramPointer);

    {
        builder.add_w12sel(_ppu->windowsel & 0xff);
        builder.add_w34sel((_ppu->windowsel & 0xff00) >> 8);
        builder.add_wobjsel((_ppu->windowsel & 0xff0000) >> 16);
    }
    {
        builder.add_wh0(_ppu->window1left);
        builder.add_wh1(_ppu->window1right);
        builder.add_wh2(_ppu->window2left);
        builder.add_wh3(_ppu->window2right);
    }

    {
        builder.add_wbglog(0);
        builder.add_wobjlog(0);
    }

    {
        builder.add_tm(_ppu->screenEnabled[0]);
        builder.add_ts(_ppu->screenEnabled[1]);
        builder.add_tmw(_ppu->screenWindowed[0]);
        builder.add_tsw(_ppu->screenWindowed[1]);
    }

    {
        ColorWindowSelect cgwsel;
        cgwsel.directSelect = 0;
        cgwsel.ccAddEnable = _ppu->addSubscreen ? ColorWindowSource::Subscreen : ColorWindowSource::FixedColor;
        cgwsel.sub = static_cast<ColorWindowFunction>(_ppu->preventMathMode);
        cgwsel.main = static_cast<ColorWindowFunction>(_ppu->clipMode);
        builder.add_cgswsel(cgwsel.raw());

        ColorAddSub cgadsub;
        cgadsub.enableMask = _ppu->mathEnabled;
        cgadsub.halfEnable = _ppu->halfColor;
        cgadsub.addSub = _ppu->subtractColor ? ColorMode::Subtraction : ColorMode::Addition;
        builder.add_cgadd(cgadsub.raw());
    }

    {
        Color5Bit fixedColor;
        fixedColor.red = _ppu->fixedColorR;
        fixedColor.green = _ppu->fixedColorG;
        fixedColor.blue = _ppu->fixedColorB;
        builder.add_fixed_color(fixedColor.asUint16());
    }
    {
        ScreenInitSettings setini;
        setini.write(0);
        builder.add_inidisp(setini.raw());
    }

    // add latches
    builder.add_mode7_latch(_ppu->m7prev);
    builder.add_ppu1_latch(_ppu->scrollPrev);
    builder.add_ppu2_latch(_ppu->scrollPrev2);

    // oam latch
    builder.add_oam_buffer(_ppu->oamBuffer);
    builder.add_oam_latch(_ppu->oamSecondWrite);

    // cgram latch
    builder.add_cgram_latch(_ppu->cgramSecondWrite);
    builder.add_cgram_buffer(_ppu->cgramBuffer);

    builder.add_extra_left_right(_ppu->extraLeftRight);

    return builder.Finish();
}

void SNESPPU::applyState(const PPUCommand::PPUState *ppuState)
{
    write(0x00, ppuState->inidisp());
    write(0x01, ppuState->objsel());
    write(0x02, ppuState->oamaddl());
    write(0x03, ppuState->oamaddh());

    write(0x05, ppuState->bgmode());
    write(0x06, ppuState->mosaic());
    write(0x07, ppuState->bg1sc());
    write(0x08, ppuState->bg2sc());
    write(0x09, ppuState->bg3sc());
    write(0x0A, ppuState->bg4sc());
    write(0x0B, ppuState->bg12nba());
    write(0x0c, ppuState->bg34nba());

    _ppu->bgLayer[0].hScroll = ppuState->bg1hofs();
    _ppu->bgLayer[0].vScroll = ppuState->bg1vofs();
    _ppu->bgLayer[1].hScroll = ppuState->bg2hofs();
    _ppu->bgLayer[1].vScroll = ppuState->bg2vofs();
    _ppu->bgLayer[2].hScroll = ppuState->bg3hofs();
    _ppu->bgLayer[2].vScroll = ppuState->bg3vofs();
    _ppu->bgLayer[3].hScroll = ppuState->bg4hofs();
    _ppu->bgLayer[3].vScroll = ppuState->bg4vofs();

    write(0x15, ppuState->vmainc());
    _ppu->vramPointer = ppuState->vmadd();

    write(0x1A, ppuState->m7sel());
    _ppu->m7matrix[0] = ppuState->m7a();
    _ppu->m7matrix[1] = ppuState->m7b();
    _ppu->m7matrix[2] = ppuState->m7c();
    _ppu->m7matrix[3] = ppuState->m7d();
    _ppu->m7matrix[4] = ppuState->m7x();
    _ppu->m7matrix[5] = ppuState->m7y();
    _ppu->m7matrix[6] = ppuState->bg1hofsmode7();
    _ppu->m7matrix[7] = ppuState->bg1vofsmode7();

    write(0x21, ppuState->cgadd());

    write(0x23, ppuState->w12sel());
    write(0x24, ppuState->w34sel());
    write(0x25, ppuState->wobjsel());
    write(0x26, ppuState->wh0());
    write(0x27, ppuState->wh1());
    write(0x28, ppuState->wh2());
    write(0x29, ppuState->wh3());
    write(0x2A, ppuState->wbglog());
    write(0x2B, ppuState->wobjlog());
    write(0x2C, ppuState->tm());
    write(0x2D, ppuState->ts());
    write(0x2E, ppuState->tmw());
    write(0x2F, ppuState->tsw());
    write(0x30, ppuState->cgswsel());
    write(0x31, ppuState->cgadd());
    auto fixedColor = Color5Bit::fromUint16(ppuState->fixed_color());
    _ppu->fixedColorR = fixedColor.red;
    _ppu->fixedColorG = fixedColor.green;
    _ppu->fixedColorB = fixedColor.blue;

    write(0x33, ppuState->setini());

    if (auto *oam = ppuState->oam())
    {
        writeOam(oam->data(), oam->size());
    }
    if (auto *cgram = ppuState->cgram())
    {
        writeCGRam(cgram->data(), cgram->size());
    }
    if (auto *vram = ppuState->vram())
    {
        memcpy(_ppu->vram, vram->data(), vram->size());
    }

    _ppu->m7prev = ppuState->mode7_latch();
    _ppu->scrollPrev = ppuState->ppu1_latch();
    _ppu->scrollPrev2 = ppuState->ppu2_latch();

    _ppu->oamBuffer = ppuState->oam_buffer();
    _ppu->oamSecondWrite = ppuState->oam_latch();

    _ppu->cgramSecondWrite = ppuState->cgram_latch();
    _ppu->cgramBuffer = ppuState->cgram_buffer();
    _ppu->extraLeftRight = ppuState->extra_left_right();
}