#include "PPUFixedFrameRecorder.hpp"

#include <sstream>

#include "utils.hpp"

std::span<uint8_t> Recording::finish()
{
    auto cmdsVector = _builder.CreateVector(_cmds);
    auto commandList = PPUCommand::CreateCommandList(_builder, cmdsVector);
    _builder.Finish(commandList);
    uint8_t *buff = _builder.GetBufferPointer();
    int size = _builder.GetSize();
    return std::span<uint8_t>(buff, size);
}

PPUFixedFrameRecorder::PPUFixedFrameRecorder(std::unique_ptr<PPUBase> ppu, int breakFrame, bool sparse) : PPURecorder(std::move(ppu)), _breakFrame(breakFrame), _sparse(sparse)
{
}

void PPUFixedFrameRecorder::beginDrawing(uint8_t *buffer, size_t pitch, uint32_t render_flags)
{
    _frameCount++;
    Parent::beginDrawing(buffer, pitch, render_flags);
}

void PPUFixedFrameRecorder::runLine(int line)
{
    Parent::runLine(line);
    if (line == 224 && _frameCount == _breakFrame)
    {
        // Write out the command list
        writeCommandList();
    }
}

void PPUFixedFrameRecorder::writeCommandList()
{
    auto data = recording().finish();
    {
        std::stringstream fileName;
        fileName << "frame" << _breakFrame;
        if (_sparse)
        {
            fileName << "_sparse";
        }
        fileName << ".ppu";
        Utils::writeBinary(fileName.str(), data, CompressionType::Zstd);
    }
    std::exit(1);
}

bool PPUFixedFrameRecorder::shouldRecordFrameDetails() const
{
    return !_sparse || _frameCount >= _breakFrame - 1;
}

bool PPUFixedFrameRecorder::shouldRecordImage() const
{
    return _frameCount == _breakFrame;
}