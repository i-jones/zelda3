#include "PPURecorder.hpp"

#include "PPUCommands_generated.h"

PPURecorder::PPURecorder(std::unique_ptr<PPUBase> ppu)
    : _ppu(std::move(ppu))
{
}

PPURecorder::~PPURecorder() = default;

void PPURecorder::reset()
{
    _ppu->reset();

    auto reset = PPUCommand::CreateReset(_recording->builder());
    _recording->addCommand(reset);
}

void PPURecorder::runLine(int line)
{
    _ppu->runLine(line);
    _recording->addCommand(_recording->builder().CreateStruct(PPUCommand::RunLine(line)));
}

uint8_t PPURecorder::read(uint8_t adr)
{

    uint8_t result = _ppu->read(adr);

    _recording->addCommand(_recording->builder().CreateStruct(PPUCommand::Read(adr, result)));

    return result;
}

void PPURecorder::write(uint8_t adr, uint8_t val)
{
    _ppu->write(adr, val);

    _recording->addCommand(_recording->builder().CreateStruct(PPUCommand::Write(adr, val)));
}

void PPURecorder::writeCGRam(const void *data, size_t size)
{
    _ppu->writeCGRam(data, size);

    const uint8_t *cgData = reinterpret_cast<const uint8_t *>(data);
    auto recordData = _recording->builder().CreateVector(cgData, size);
    auto writeCGRam = PPUCommand::CreateWriteCGRam(_recording->builder(), recordData);
    _recording->addCommand(writeCGRam);
}

void PPURecorder::writeOam(const void *data, size_t size)
{
    _ppu->writeOam(data, size);

    const uint8_t *oamData = reinterpret_cast<const uint8_t *>(data);
    auto recordData = _recording->builder().CreateVector(oamData, size);
    auto writeOam = PPUCommand::CreateWriteCGRam(_recording->builder(), recordData);
    _recording->addCommand(writeOam);
}

void PPURecorder::saveLoad(PpuSaveLoadFunc *func, void *context)
{
    _ppu->saveLoad(func, context);
}

void PPURecorder::beginDrawing(uint8_t *buffer, size_t pitch, uint32_t render_flags)
{
    _ppu->beginDrawing(buffer, pitch, render_flags);

    auto beginDrawing = PPUCommand::CreateBeginDrawing(_recording->builder());
    _recording->addCommand(beginDrawing);
}

int PPURecorder::getCurrentRenderScale(uint32_t render_flags)
{
    int result = _ppu->getCurrentRenderScale(render_flags);

    _recording->addCommand(_recording->builder().CreateStruct(PPUCommand::GetCurrentRenderScale(render_flags, result)));

    return result;
}
void PPURecorder::setMode7PerspectiveCorrection(int low, int high)
{
    _ppu->setMode7PerspectiveCorrection(low, high);

    _recording->addCommand(_recording->builder().CreateStruct(PPUCommand::SetMode7PerspectiveCorrection(low, high)));
}

void PPURecorder::setExtraSideSpace(int left, int right, int bottom)
{
    _ppu->setExtraSideSpace(left, right, bottom);

    _recording->addCommand(_recording->builder().CreateStruct(PPUCommand::SetExtraSideSpace(left, right, bottom)));
}

uint8_t PPURecorder::getMode()
{
    uint8_t result = _ppu->getMode();

    _recording->addCommand(_recording->builder().CreateStruct(PPUCommand::GetMode(result)));

    return result;
}

uint8_t PPURecorder::getExtraLeftRight()
{
    uint8_t result = _ppu->getExtraLeftRight();

    _recording->addCommand(_recording->builder().CreateStruct(PPUCommand::GetExtraLeftRight(result)));

    return result;
}

void PPURecorder::setExtraLeftRight(uint8_t extraLeftRight)
{
    _ppu->setExtraLeftRight(extraLeftRight);

    _recording->addCommand(_recording->builder().CreateStruct(PPUCommand::SetExtraLeftRight(extraLeftRight)));
}

Ppu *PPURecorder::getPpu()
{
    return _ppu->getPpu();
}