#include "PPUFixedFrameRecorder.hpp"

#include <fstream>

std::span<uint8_t> Recording::finish()
{
    auto cmdsVector = _builder.CreateVector(_cmds);
    auto commandList = PPUCommand::CreateCommandList(_builder, cmdsVector);
    _builder.Finish(commandList);
    uint8_t *buff = _builder.GetBufferPointer();
    int size = _builder.GetSize();
    return std::span<uint8_t>(buff, size);
}

PPUFixedFrameRecorder::PPUFixedFrameRecorder(std::unique_ptr<PPUBase> ppu, int breakFrame) : PPURecorder(std::move(ppu)), _breakFrame(breakFrame)
{
}

void PPUFixedFrameRecorder::beginDrawing(uint8_t *buffer, size_t pitch, uint32_t render_flags)
{
    Parent::beginDrawing(buffer, pitch, render_flags);
    _frameCount++;
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
        std::ofstream recordingFile("test.ppu", std::ios::binary);
        recordingFile.write(reinterpret_cast<char *>(data.data()), data.size_bytes());
    }
    std::exit(1);
}