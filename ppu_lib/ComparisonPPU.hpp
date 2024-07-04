#pragma once

#include <memory>

#include "PPUBase.hpp"
#include "PPURecorder.hpp"

class ComparisonPPU : public PPUBase
{
public:
    ComparisonPPU(
        std::unique_ptr<PPUBase> primary,
        std::unique_ptr<PPUBase> secondary);

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

    flatbuffers::Offset<PPUCommand::PPUState> createState(flatbuffers::FlatBufferBuilder &builder) override;
    void applyState(const PPUCommand::PPUState *PPUState) override;

private:
    bool compareLine(int lint) const;

    std::unique_ptr<PPURecorder> _primary;
    std::unique_ptr<PPUBase> _secondary;
    std::vector<uint8_t> _secondayRenderBuffer;
    std::span<uint8_t> _primayRenderBuffer;
    bool _imagesDiffer = false;
    int _frameCount = 0;
};