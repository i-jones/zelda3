#pragma once

#include <memory>
#include <span>

#include "PPUBase.hpp"

#include "flatbuffers/flatbuffers.h"
#include "PPUCommands_generated.h"

class Recording
{
public:
    Recording() = default;

    template <typename T>
    void addCommand(flatbuffers::Offset<T> offset)
    {
        using CommandType = std::remove_const_t<std::remove_pointer_t<T>>;
        auto cmdType = PPUCommand::CommandTypeTraits<CommandType>::enum_value;
        if (cmdType == PPUCommand::CommandType_NONE)
        {
            throw std::runtime_error("Invalid command type");
        }

        flatbuffers::Offset<PPUCommand::Command> cmd = PPUCommand::CreateCommand(_builder, cmdType, offset.Union());

        _cmds.push_back(std::move(cmd));
    }

    flatbuffers::FlatBufferBuilder &builder() { return _builder; }

    std::span<uint8_t> finish();

private:
    flatbuffers::FlatBufferBuilder _builder;
    std::vector<flatbuffers::Offset<PPUCommand::Command>> _cmds;
};

class PPURecorder : public PPUBase
{
public:
    PPURecorder(std::unique_ptr<PPUBase> ppu);
    ~PPURecorder();

    void reset() override;
    void runLine(int line) override;
    uint8_t read(uint8_t adr) override;
    void write(uint8_t adr, uint8_t val) override;
    void writeCGRam(const void *data, size_t size) override;
    void writeOam(const void *data, size_t size) override;
    void saveLoad(PpuSaveLoadFunc *func, void *context) override;
    void beginDrawing(uint8_t *buffer, size_t pitch, uint32_t render_flags) override;
    int getCurrentRenderScale(uint32_t render_flags) override;
    void setMode7PerspectiveCorrection(int low, int high) override;
    void setExtraSideSpace(int left, int right, int bottom) override;
    uint8_t getMode() override;
    uint8_t getExtraLeftRight() override;
    void setExtraLeftRight(uint8_t extraLeftRight) override;
    Ppu *getPpu() override;

    Recording &recording() { return *_recording; }

private:
    std::unique_ptr<PPUBase> _ppu;
    std::unique_ptr<Recording> _recording;
};