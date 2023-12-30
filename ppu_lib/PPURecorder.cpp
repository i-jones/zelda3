#include "PPURecorder.hpp"

#include <algorithm>

#include "utils.hpp"

static_assert(FLATBUFFERS_LITTLEENDIAN);

PPURecorder::PPURecorder(std::unique_ptr<PPUBase> ppu)
    : _ppu(std::move(ppu))
{
    vram = _ppu->vram;
    initRecording(false);
}

PPURecorder::~PPURecorder() = default;

void PPURecorder::reset()
{
    _ppu->reset();

    if (!_recording)
        return;

    auto reset = PPUCommand::CreateReset(_recording->builder());
    _recording->addCommand(reset);
}

void PPURecorder::runLine(int line)
{
    _ppu->runLine(line);
    if (!_recording)
        return;

    if (!shouldRecordImage())
        return;
    _recording->addCommand(_recording->builder().CreateStruct(PPUCommand::RunLine(line)));

    if (line == 224 && _drawBuffer && shouldRecordImage())
    {
        size_t drawBufferSize = 224 * 256 * 4;
        auto imageData = _recording->builder().CreateVector(_drawBuffer, drawBufferSize);
        auto validateImage = PPUCommand::CreateValidateImage(_recording->builder(), imageData);
        _recording->addCommand(validateImage);
    }
}

uint8_t PPURecorder::read(uint8_t adr)
{

    uint8_t result = _ppu->read(adr);

    if (_recording)
    {
        _recording->addCommand(_recording->builder().CreateStruct(PPUCommand::Read(adr, result)));
    }
    return result;
}

void PPURecorder::write(uint8_t adr, uint8_t val)
{
    _ppu->write(adr, val);
    if (!_recording)
        return;

    _recording->addCommand(_recording->builder().CreateStruct(PPUCommand::Write(adr, val)));
}

void PPURecorder::writeCGRam(const void *data, size_t size)
{

    _ppu->writeCGRam(data, size);
    if (!_recording)
        return;

    if (!shouldRecordFrameDetails())
        return;

    const uint8_t *cgData = reinterpret_cast<const uint8_t *>(data);
    if (cgramData.update({cgData, size}))
    {
        auto recordData = _recording->builder().CreateVector(cgData, size);
        auto writeCGRam = PPUCommand::CreateWriteCGRam(_recording->builder(), recordData);
        _recording->addCommand(writeCGRam);
    }
}

void PPURecorder::writeOam(const void *data, size_t size)
{
    _ppu->writeOam(data, size);
    if (!_recording)
        return;

    if (!shouldRecordFrameDetails())
        return;

    const uint8_t *oamData = reinterpret_cast<const uint8_t *>(data);
    if (this->oamData.update({oamData, size}))
    {
        auto recordData = _recording->builder().CreateVector(oamData, size);
        auto writeOam = PPUCommand::CreateWriteOAM(_recording->builder(), recordData);
        _recording->addCommand(writeOam);
    }
}

void PPURecorder::saveLoad(PpuSaveLoadFunc *func, void *context)
{
    _ppu->saveLoad(func, context);
}

void PPURecorder::beginDrawing(uint8_t *buffer, size_t pitch, uint32_t render_flags)
{
    _ppu->beginDrawing(buffer, pitch, render_flags);
    if (!_recording)
        return;

    if (!shouldRecordImage())
        return;
    _drawBuffer = buffer;

    flatbuffers::Offset<flatbuffers::Vector<uint8_t>> vramData{};
    if (this->vramData.update({reinterpret_cast<const uint8_t *>(vram), sizeof(PpuVRam)}))
    {
        vramData = _recording->builder().CreateVector(reinterpret_cast<uint8_t *>(vram), sizeof(PpuVRam));
    }
    auto beginDrawing = PPUCommand::CreateBeginDrawing(_recording->builder(), 0, 0, render_flags, vramData);
    _recording->addCommand(beginDrawing);
}

int PPURecorder::getCurrentRenderScale(uint32_t render_flags)
{
    int result = _ppu->getCurrentRenderScale(render_flags);

    if (_recording)
    {
        _recording->addCommand(_recording->builder().CreateStruct(PPUCommand::GetCurrentRenderScale(render_flags, result)));
    }
    return result;
}
void PPURecorder::setMode7PerspectiveCorrection(int low, int high)
{
    _ppu->setMode7PerspectiveCorrection(low, high);
    if (!_recording)
        return;

    _recording->addCommand(_recording->builder().CreateStruct(PPUCommand::SetMode7PerspectiveCorrection(low, high)));
}

void PPURecorder::setExtraSideSpace(int left, int right, int bottom)
{
    _ppu->setExtraSideSpace(left, right, bottom);
    if (!_recording)
        return;

    _recording->addCommand(_recording->builder().CreateStruct(PPUCommand::SetExtraSideSpace(left, right, bottom)));
}

uint8_t PPURecorder::getMode()
{
    uint8_t result = _ppu->getMode();

    if (_recording)
    {
        _recording->addCommand(_recording->builder().CreateStruct(PPUCommand::GetMode(result)));
    }
    return result;
}

uint8_t PPURecorder::getExtraLeftRight()
{
    uint8_t result = _ppu->getExtraLeftRight();

    if (_recording)
    {
        _recording->addCommand(_recording->builder().CreateStruct(PPUCommand::GetExtraLeftRight(result)));
    }
    return result;
}

void PPURecorder::setExtraLeftRight(uint8_t extraLeftRight)
{
    _ppu->setExtraLeftRight(extraLeftRight);
    if (!_recording)
        return;

    _recording->addCommand(_recording->builder().CreateStruct(PPUCommand::SetExtraLeftRight(extraLeftRight)));
}

Ppu *PPURecorder::getPpu()
{
    return _ppu->getPpu();
}

bool PPURecorder::shouldRecordFrameDetails() const
{
    return true;
}

bool PPURecorder::shouldRecordImage() const
{
    return true;
}

bool PPURecorder::BufferState::update(std::span<const uint8_t> data)
{
    std::span<const uint8_t> current(_data.data(), _data.size());
    if (std::equal(current.begin(), current.end(), data.begin(), data.end()))
    {
        return false;
    }
    _data.resize(data.size());
    std::copy(data.begin(), data.end(), _data.begin());
    assert(std::equal(_data.begin(), _data.end(), data.begin(), data.end()));
    return true;
}

void PPURecorder::initRecording(bool captureState)
{
    if (_recording)
    {
        return;
    }
    _recording = std::make_unique<Recording>();
    if (captureState)
    {
        auto state = _ppu->createState(_recording->builder());
        _recording->addCommand(state);
    }
}

flatbuffers::Offset<PPUCommand::PPUState> PPURecorder::createState(flatbuffers::FlatBufferBuilder &builder)
{
    return _ppu->createState(builder);
}

void PPURecorder::applyState(const PPUCommand::PPUState *PPUState)
{
    _ppu->applyState(PPUState);
}

bool PPURecorder::endRecording(std::optional<std::string_view> name)
{
    if (!_recording)
    {
        return false;
    }

    bool result = false;
    if (name)
    {
        auto data = _recording->finish();
        result = Utils::writeBinary(*name, data, CompressionType::Zstd);
    }
    _recording.reset();
    return result;
}