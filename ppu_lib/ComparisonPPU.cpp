#include "ComparisonPPU.hpp"

#include <sstream>

#include "color.hpp"

ComparisonPPU::ComparisonPPU(
    std::unique_ptr<PPUBase> primary,
    std::unique_ptr<PPUBase> secondary)
    : _primary(std::make_unique<PPURecorder>(std::move(primary))), _secondary(std::move(secondary))
{
    _primary->endRecording(std::nullopt);
    vram = _primary->vram;
}

void ComparisonPPU::reset()
{
    _primary->reset();
    _secondary->reset();
}

void ComparisonPPU::runLine(int line)
{
    // Copy vram to secondary
    memcpy(_secondary->vram, _primary->vram, sizeof(PpuVRam));
    _primary->runLine(line);
    _secondary->runLine(line);
    _imagesDiffer = _imagesDiffer || !compareLine(line);
    if (line == 224)
    {
        if (_imagesDiffer)
        {
            std::stringstream fileName;
            fileName << "diffFrame" << _frameCount;
            fileName << ".ppu";

            _primary->endRecording(fileName.str());
        }
        else
        {
            _primary->endRecording(std::nullopt);
        }
    }
}

uint8_t ComparisonPPU::read(uint8_t adr)
{
    auto result = _primary->read(adr);
    auto result2 = _secondary->read(adr);
    return result;
}
void ComparisonPPU::write(uint8_t adr, uint8_t val)
{
    _primary->write(adr, val);
    _secondary->write(adr, val);
}

void ComparisonPPU::writeCGRam(const void *data, size_t size)
{
    _primary->writeCGRam(data, size);
    _secondary->writeCGRam(data, size);
}

void ComparisonPPU::writeOam(const void *data, size_t size)
{
    _primary->writeOam(data, size);
    _secondary->writeOam(data, size);
}

void ComparisonPPU::saveLoad(PpuSaveLoadFunc *func, void *context)
{
    throw std::runtime_error("Not implemented");
}

void ComparisonPPU::beginDrawing(uint8_t *buffer, size_t pitch, uint32_t render_flags)
{
    _frameCount++;
    _primary->initRecording();
    _imagesDiffer = false;
    assert(pitch == sizeof(OutputPixelFormat) * 256);
    _primayRenderBuffer = std::span<uint8_t>(buffer, pitch * 224);
    _primary->beginDrawing(buffer, pitch, render_flags);
    assert(pitch == 256 * 4);
    _secondayRenderBuffer.resize(224 * pitch);
    _secondary->beginDrawing(_secondayRenderBuffer.data(), pitch, render_flags);
}

int ComparisonPPU::getCurrentRenderScale(uint32_t render_flags)
{
    auto result = _primary->getCurrentRenderScale(render_flags);
    auto other = _secondary->getCurrentRenderScale(render_flags);
    return result;
}

void ComparisonPPU::setMode7PerspectiveCorrection(int low, int high)
{
    _primary->setMode7PerspectiveCorrection(low, high);
    _secondary->setMode7PerspectiveCorrection(low, high);
}

void ComparisonPPU::setExtraSideSpace(int left, int right, int bottom)
{
    _primary->setExtraSideSpace(left, right, bottom);
    _secondary->setExtraSideSpace(left, right, bottom);
}

uint8_t ComparisonPPU::getMode()
{
    auto result = _primary->getMode();
    auto other = _secondary->getMode();
    return result;
}

uint8_t ComparisonPPU::getExtraLeftRight()
{
    auto result = _primary->getExtraLeftRight();
    auto other = _secondary->getExtraLeftRight();
    return result;
}

void ComparisonPPU::setExtraLeftRight(uint8_t extraLeftRight)
{
    _primary->setExtraLeftRight(extraLeftRight);
    _secondary->setExtraLeftRight(extraLeftRight);
}

Ppu *ComparisonPPU::getPpu()
{
    return _primary->getPpu();
}

flatbuffers::Offset<PPUCommand::PPUState> ComparisonPPU::createState(flatbuffers::FlatBufferBuilder &builder)
{
    return _primary->createState(builder);
}

void ComparisonPPU::applyState(const PPUCommand::PPUState *PPUState)
{
    _primary->applyState(PPUState);
    _secondary->applyState(PPUState);
}

bool ComparisonPPU::compareLine(int line) const
{
    if (line >= 224)
    {
        return true;
    }
    auto lineOffst = line * 256;
    std::span<const OutputPixelFormat> primary(reinterpret_cast<const OutputPixelFormat *>(_primayRenderBuffer.data()) + lineOffst, 256);
    std::span<const OutputPixelFormat> secondary(reinterpret_cast<const OutputPixelFormat *>(_secondayRenderBuffer.data()) + lineOffst, 256);

    for (int i = 0; i < primary.size(); i++)
    {
        if (primary[i] != secondary[i])
        {
            return false;
            /*
            primary[i].r() = 255;
            primary[i].g() = 0;
            primary[i].b() = 255;
            */
        }
    }
    return true;
}